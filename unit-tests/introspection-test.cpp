/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include <unistd.h>
#include <dbus-cxx.h>

#include "test_macros.h"

std::shared_ptr<DBus::Dispatcher> dispatch;
std::shared_ptr<DBus::Connection> conn;

std::shared_ptr<DBus::ObjectProxy> proxy;
std::shared_ptr<DBus::MethodProxy<int(int,int)>> int_method_proxy;
std::shared_ptr<DBus::ObjectProxy> introspectionProxy;
std::shared_ptr<DBus::MethodProxy<std::string()>> introspection_method_proxy;

std::shared_ptr<DBus::Object> object;
std::shared_ptr<DBus::Method<int(int,int)>> int_method;

int add( int, int ){
    return 1;
}

bool introspect_basic_introspect(){
    std::string introspectionData = (*introspection_method_proxy)();

    return introspectionData.size() > 10;
}

void client_setup(){
    proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );

    int_method_proxy = proxy->create_method<int(int,int)>( "foo.what", "add" );

    introspectionProxy = conn->create_object_proxy( "dbuscxx.test", "/test" );
    introspection_method_proxy = proxy->create_method<std::string()>( "org.freedesktop.DBus.Introspectable", "Introspect" );
}

void server_setup(){
    int ret = conn->request_name( "dbuscxx.test", DBUS_NAME_FLAG_REPLACE_EXISTING );
    if( ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER ) exit(1);

    object = conn->create_object("/test");
    int_method = object->create_method<int(int,int)>("foo.what", "add", sigc::ptr_fun( add ) );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
  ret = introspect_##name();\
} \
} while( 0 )

int main(int argc, char** argv){
  if(argc < 1)
    return 1;

  std::string test_name = argv[2];
  bool ret = false;
  bool is_client = std::string( argv[1] ) == "client";

  DBus::init();
  dispatch = DBus::Dispatcher::create();
  conn = dispatch->create_connection(DBus::BusType::SESSION);

  if( is_client ){
    client_setup();
    ADD_TEST(basic_introspect);
  }else{
    server_setup();
    ret = true;
    sleep( 1 );
  }


  return !ret;
}
