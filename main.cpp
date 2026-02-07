#include <nodepp/nodepp.h>
#include <nodepp/json.h>
#include <nodepp/http.h>
#include <torify/http.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void resolve_osocket_1( http_t& cli ) {

    auto data = regex::split( cli.path, ":" );
    auto skt  = torify::tcp::client();

    skt.onOpen([=]( socket_t raw ){
        cli.write_header( 200, header_t({  }) );
        stream::duplex  ( raw, cli );
    });

    skt.onError([=]( except_t ){
        cli.write_header( 404, header_t({  }) );
        cli.write( "couldn't connect to url" );
    });

    skt.connect( 
        /*------------*/ data[0] ,
        string::to_uint( data[1] )
    );

}

void resolve_osocket_2( http_t& cli ) {

    torify_fetch_t args;
    /*----------*/ args.url    = cli.path;
    /*----------*/ args.method = cli.method;
    /*----------*/ args.headers= cli.headers;

    torify::http::fetch( args )
    
    .then([=]( http_t raw ){
        cli.write_header( raw.status, raw.headers );
        stream::duplex( raw, cli );
    })

    .fail([=]( except_t err ){
        cli.write_header( 404, header_t({  }) );
        cli.write( "couldn't connect to url" );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/

void resolve_nsocket_1( http_t& cli ) {
    
    auto data = regex::split( cli.path, ":" );
    auto skt  = tcp::client();

    skt.onOpen([=]( socket_t raw ){
        cli.write_header( 200, header_t({  }) );
        stream::duplex( raw, cli );
    });

    skt.onError([=]( except_t ){
        cli.write_header( 404, header_t({  }) );
        cli.write( "couldn't connect to url" );
    });

    skt.connect( 
        dns::lookup/**/( data[0] ),
        string::to_uint( data[1] )
    );

}

void resolve_nsocket_2( http_t& cli ) {

    fetch_t args;
    /*---*/ args.url    = cli.path;
    /*---*/ args.method = cli.method;
    /*---*/ args.headers= cli.headers;

    http::fetch( args )
    
    .then([=]( http_t raw ){
        cli.write_header( raw.status, raw.headers );
        stream::duplex( raw, cli );
    })

    .fail([=]( except_t ){
        cli.write_header( 404, header_t({  }) );
        cli.write( "couldn't connect to url" );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    auto server = http::server([]( http_t cli ){

        if( cli.method == "CONNECT" ){

            if( regex::test( cli.path, "[.]onion" ) )
                 { resolve_osocket_1( cli ); } 
            else { resolve_nsocket_1( cli ); }
            
        } elif( url::is_valid( cli.path ) ) {

            if( regex::test( cli.path, "[.]onion" ) )
                 { resolve_osocket_2( cli ); } 
            else { resolve_nsocket_2( cli ); }

        } else {

            console::log( cli.method, cli.path );
            cli.write_header( 404, header_t({  }) );
            cli.write( "invalid url" );

        }

    });

    server.listen( "0.0.0.0", 5090 ,[]( ... ){
        console::log( "Listenning http://localhost:5090" );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/