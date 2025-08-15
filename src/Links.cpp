
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
using namespace geode::prelude;

#include <regex>

//data values
inline static auto server = std::string(
	"gtps.bccst.ru/game/"
);
inline static auto links = matjson::parse(R"({
	"asdasd": "asdasd",
	"https://www.robtopgames.com": "https://t.me/user95401_channel",
	"https://www.boomlings.com/database/accounts/accountManagement.php": "https://gtps.bccst.ru/dashboard/",
	"https://store.steampowered.com/recommended/recommendgame/322170": "https://gdpshub.com/gdps/900",
	"https://discord.com/invite/geometrydash": "https://discord.gg/UyQytJsrGZ",
	"https://twitter.com/robtopgames": "https://t.me/user95401_channel",
	"https://www.youtube.com/user/RobTopGames": "https://www.youtube.com/channel/UCpgyf8EpL62WyDM2jWo4v1w"
})").unwrapOrDefault();

//send
#include <Geode/modify/CCHttpClient.hpp>
class $modify(CCHttpClientLinksReplace, CCHttpClient) {
	$override void send(CCHttpRequest * req) {
		std::string url = req->getUrl();
		url = string::replace(url, "www.boomlings.com/database", server);
		url = string::replace(url, "boomlings.com/database", server);
		if (getMod()->getSettingValue<bool>("redir request urls")) req->setUrl(url.c_str());
		return CCHttpClient::send(req);
	}
};

//url open
#include <Geode/modify/CCApplication.hpp>
class $modify(CCApplicationLinksReplace, CCApplication) {
	$override void openURL(const char* url) {
		url = not links.contains(url) ? url : links[url].asString().unwrapOr(url).c_str();
		url = string::replace(url, "https://www.twitter.com/", "https://t.me/").c_str();
		url = string::replace(url, "www.boomlings.com/database", server).c_str();
		url = string::replace(url, "boomlings.com/database", server).c_str();
		//log::debug("{}.url = {}", __FUNCTION__, url);
		return CCApplication::openURL(url);
	}
};