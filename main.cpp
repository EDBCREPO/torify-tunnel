#define NODEPP_ALLOW_THROW_EXCEPTION 0

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h>
#include <nodepp/http.h>
#include <torify/http.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void resolve_osocket_1( http_t& cli ) { do {
auto data = regex::split( cli.path, ":" );

    if( data.size() < 2 ){ break; }

    auto host = data[0];
    auto skt  = torify::tcp::client();
    auto port = string::to_uint( data[1] );

    skt.onOpen.once([=]( socket_t raw ){
        cli.write_header( 200, header_t({  }) );
        stream::duplex  ( raw, cli );
    });

    skt.onError.once([=]( except_t err ){
        auto message = string::join( " ", "couldn't connect to url", err.what() );
        cli.write_header( 404, header_t({  }) );
        cli.write( message );
    });

    skt.connect( host, port ); 
    
return; } while(0); 

    cli.write_header( 404, header_t({  }) );
    cli.write( "couldn't connect to url" );

}

void resolve_osocket_2( http_t& cli ) {

    torify_fetch_t args;

    /*----------*/ args.proxy  = "tcp://localhost:9050";
    /*----------*/ args.url    = cli.path   ;
    /*----------*/ args.method = cli.method ;
    /*----------*/ args.headers= cli.headers;

    torify::http::fetch( args )
    
    .then([=]( http_t raw ){
        cli.write_header( raw.status, raw.headers );
        stream::duplex( raw, cli );
    })

    .fail([=]( except_t err ){
        auto message = string::join( " ", "couldn't connect to url", err.what() );
        cli.write_header( 404, header_t({  }) );
        cli.write( message );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/

void resolve_nsocket_1( http_t& cli ) { do {
auto data = regex::split( cli.path, ":" );

    if( data.size() < 2 ){ break; }

    auto dns  = dns::lookup/**/( data[0] );
    auto port = string::to_uint( data[1] );
    auto skt  = tcp::client();

    if( dns.empty() ){ return; }

    skt.onOpen.once([=]( socket_t raw ){
        cli.write_header( 200, header_t({  }) );
        stream::duplex( raw, cli );
    });

    skt.onError.once([=]( except_t err ){
        auto message = string::join( " ", "couldn't connect to url", err.what() );
        cli.write_header( 404, header_t({  }) ); cli.write( message );
    });

    skt.connect( dns[0], port ); 
    
return; } while(0); 

    cli.write_header( 404, header_t({  }) );
    cli.write( "couldn't connect to url" );

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

    .fail([=]( except_t err ){
        auto message = string::join( " ", "couldn't connect to url", err.what() );
        cli.write_header( 404, header_t({  }) ); cli.write( message );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    auto server = http::server([]( http_t cli ){

        console::log( ">>", cli.get_fd(), cli.method, cli.path );

        if( cli.method == "CONNECT" ){

            if   ( regex::test( cli.path, "[.]onion" ) )
                 { resolve_osocket_1( cli ); } 
            else { resolve_nsocket_1( cli ); }
            
        } elif( url::is_valid( cli.path ) ) {

            if   ( regex::test( cli.path, "[.]onion" ) )
                 { resolve_osocket_2( cli ); } 
            else { resolve_nsocket_2( cli ); }

        } else {

            cli.write_header( 404, header_t({  }) );
            cli.write( "invalid url" );

        }

    });

    server.listen( "0.0.0.0", 5090 ,[]( ... ){
        console::log( "Listenning http://localhost:5090" );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/