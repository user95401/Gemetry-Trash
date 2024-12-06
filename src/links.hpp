#pragma once
#include <_main.hpp>

using namespace geode::prelude;

//data values

inline static auto server = fs::path();

inline static auto links = matjson::Value();

//get data

inline void loadServerInf() {
	if (not server.empty()) return;
	auto req = web::WebRequest();
	auto listener = new EventListener<web::WebTask>;
	listener->bind(
		[](web::WebTask::Event* e) {
			if (web::WebResponse* res = e->getValue()) {
				std::string data = res->string().unwrapOr("no res");
				if ((res->code() < 399) and (res->code() > 10)) {
					server = std::regex_replace(data, std::regex("\\s"), "");
					log::debug("server = {}", server);
				}
				else {
				}
			}
		}
	);
	listener->setFilter(req.send(
		"GET", (raw_content_repo_lnk + "/data/" + "/server.inf")
	));
}

$on_mod(Loaded){
	loadServerInf();
}

inline void loadLinks() {
	auto req = web::WebRequest();
	auto listener = new EventListener<web::WebTask>;
	listener->bind(
		[](web::WebTask::Event* e) {
			if (web::WebResponse* res = e->getValue()) {
				std::string data = res->string().unwrapOr("no res");
				auto parse = res->json();
				if ((res->code() < 399) and (res->code() > 10) and parse.isOk()) {
					links = parse.unwrapOrDefault();
					log::debug("links = {}", links.dump());
				}
				else {
				}
			}
		}
	);
	listener->setFilter(req.send(
		"GET", (raw_content_repo_lnk + "/data/" + "/url.list.json")
	));
}

$on_mod(Loaded){
	loadLinks();
}

//send

#include <Geode/modify/CCHttpClient.hpp>
class $modify(CCHttpClientLinksReplace, CCHttpClient) {
	$override void send(CCHttpRequest * req) {
		std::string url = req->getUrl();
		if (not server.empty()) url = std::regex_replace(
			url, 
			std::regex("www.boomlings.com\\/database"), 
			server.string()
		);
		log::debug("{}.url = {}", __FUNCTION__, url);
		req->setUrl(url.c_str());
		return CCHttpClient::send(req);
	}
};

/*
inline web::WebTask web_send_replace(web::WebRequest* __this, std::string_view method, std::string_view url) {
	auto newUrl = std::string(url.data());
	if (not server.empty()) newUrl = std::regex_replace(
		newUrl.data(),
		std::regex("www.boomlings.com\\/database"),
		server.string()
	);
	log::debug("{}.newUrl = {}", __FUNCTION__, newUrl);
	return __this->send(method, std::string_view(newUrl.data()));
};

$on_mod(Loaded){
	auto hook = Mod::get()->hook(
		reinterpret_cast<void*>(
			geode::addresser::getNonVirtual(&web::WebRequest::send)
		),
		&web_send_replace,
		"web::WebRequest::send",
		tulip::hook::TulipConvention::Thiscall
	);
	if (hook.has_error()) log::error("hook failed: {}", hook.error_or("no error..."));
	if (hook.has_value()) log::debug("{}", hook.value()->getRuntimeInfo().dump(4));
};
*/

//url open

#include <Geode/modify/CCApplication.hpp>
class $modify(CCApplicationLinksReplace, CCApplication) {
	$override void openURL(const char* url) {
		if (links.contains(url)) {
			url = links[url].asString().unwrapOr(url).c_str();
		}
		//log::debug("{}.url = {}", __FUNCTION__, url);
		return CCApplication::openURL(url);
	}
};