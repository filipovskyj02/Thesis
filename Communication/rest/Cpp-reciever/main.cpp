#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/submit_order")
       .methods("POST"_method)
   ([](const crow::request& req){
       auto x = crow::json::load(req.body);
       if (!x)
           return crow::response(400);
       //int id = x["id"].i();
       std::ostringstream os;
       os << 2;
       return crow::response{os.str()};
   });

    CROW_ROUTE(app, "/hello")
   ([](){
       return "Hello World!";
   });

    app.port(8080)
    .multithreaded()
    .run_async();
}