#include "raii.h"

oosl::common::raii::raii(callback_type init, callback_type uninit)
	: uninit_(uninit){
	init();
}

oosl::common::raii::~raii(){
	uninit_();
}
