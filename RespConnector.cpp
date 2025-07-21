#include "RespConnector.h"

simdjson::dom::parser RespConnector::parser;
RespProcessor*  RespConnector::mRespProcessor;

void RespConnector::on_open(websocketpp::connection_hdl hdl, client* c) {
    std::cout << "WebSocket connection opened!" << std::endl;
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c->get_con_from_hdl(hdl, ec);
 
    if (ec) {
        std::cout << "Failed to get connection pointer: " << ec.message() << std::endl;
        return;
    }
    simdjson::dom::element doc = RespConnector::parser.load("/home/git_repos/Confs/MD_auth.json");
    std::stringstream ss;
    ss << R"({"action": "auth", "key": ")" << doc["key"].get_string() << R"(", "secret": ")" << doc["secret"].get_string() << R"("})"_padded;
    auto auth_message_json = ss.str();
    char *auth_message = auth_message_json.data();
    c->send(con, auth_message, websocketpp::frame::opcode::text);

    auto subscription_message_json = R"({"action":"listen","data":{ "streams":["trade_updates"] }})"_padded;
    char *subscription_message = subscription_message_json.data();
    c->send(con, subscription_message, websocketpp::frame::opcode::text);
}

void RespConnector::on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;

    simdjson::padded_string padded_json_string(msg->get_payload());

    for(const auto& key_value : RespConnector::parser.parse(padded_json_string).get_object()){
        if(key_value.key == "data"){
            simdjson::dom::object obj = key_value.value;
            for(const auto& key_value2 : obj) {
                if(key_value2.key == "event"){
                    std::string_view value = key_value.value["event"].get_string();
                    if(value == "new"){
                        mRespProcessor->process_order_confirm(obj);
                        continue;
                    }
                    else if(value == "fill" || value == "partial_fill"){
                        mRespProcessor->process_fill(obj);
                        continue;
                    }
                    else if(value == "canceled" || value == "suspended" || value == "expired" || value == "done_for_day"){
                        mRespProcessor->process_cancel(obj);
                        continue;
                    }
                    else if(value == "replaced"){
                        mRespProcessor->process_replace(obj);
                        continue;
                    }
                    else if(value == "rejected"){
                        mRespProcessor->process_order_reject(obj);
                        continue;
                    }
                    else if(value == "order_replace_rejected"){
                        mRespProcessor->process_replace_reject(obj);
                        continue;
                    }
                    else if(value == "order_cancel_rejected"){
                        mRespProcessor->process_cancel_reject(obj);
                        continue;
                    }
                }
            }
        }
    }
}

void RespConnector::on_fail(websocketpp::connection_hdl hdl) {
    std::cout << "WebSocket connection failed!" << std::endl;
}

void RespConnector::on_close(websocketpp::connection_hdl hdl) {
    std::cout << "WebSocket connection closed!" << std::endl;
}

void RespConnector::on_init() {

    mShmemManager = ShmemManager::getInstance();
    mSymIDManager = SymbolIDManager::getInstance();
    mRespProcessor = RespProcessor::getInstance();

    mShmemManager->startUp();
    mSymIDManager->startUp();
    mRespProcessor->startUp();
    
    try {
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        c.set_error_channels(websocketpp::log::elevel::all);
        c.init_asio();
      
        c.set_message_handler(&on_message);
        c.set_tls_init_handler(bind(&on_tls_init, hostname.c_str(), ::_1));
       
        c.set_open_handler(bind(&on_open, ::_1, &c));
        c.set_fail_handler(bind(&on_fail, ::_1));
        c.set_close_handler(bind(&on_close, ::_1));
        c.set_error_channels(websocketpp::log::elevel::all);  // Enable detailed error logging
        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "Could not create connection because: " << ec.message() << std::endl;
            return;
        }
        c.connect(con);
       
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cout << "WebSocket Exception: " << e.what() << std::endl;
    }
}