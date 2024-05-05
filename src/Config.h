#ifndef QUICKWEBSHORTCUTS_CONFIG_H
#define QUICKWEBSHORTCUTS_CONFIG_H

struct Config {
    constexpr static const auto RootGroup = "Config";
    constexpr static const auto ConfigFile = "quickwebshortcutsrunnerrc";

    // General config
    constexpr static const auto SearchEngineName = "search_engine_name";
    constexpr static const auto ShowSearchForNote = "show_search_for_note";
    constexpr static const auto ShowName = "show_name";
    constexpr static const auto PrivateWindowNote = "private_window_note";
    constexpr static const auto PrivateWindowAction = "private_window_action";
    constexpr static const auto TriggerCharacter = "trigger_character";
    constexpr static const auto TriggerCharacterDefault = ":";
    constexpr static const auto WebShortcut = "web_shortcut";

    // Search suggestions
    constexpr static const auto SearchSuggestions = "search_suggestions";
    constexpr static const auto SearchSuggestionGoogle = "google";
    constexpr static const auto SearchSuggestionBing = "bing";
    constexpr static const auto SearchSuggestionDuckDuckGo = "duckduckgo";
    constexpr static const auto SearchSuggestionDisabled = "disabled";

    constexpr static const auto PrivateWindowSearchSuggestions = "private_window_search_suggestions";
    constexpr static const auto MinimumLetterCount = "minimum_letter_count";
    static const int MinimumLetterCountDefault;
    constexpr static const auto MaxSuggestionResults = "max_search_suggestions";
    static const int MaxSuggestionResultsDefault;
    constexpr static const auto BingMarket = "bing_locale";
    constexpr static const auto BingMarketDefault = "en-us";
    constexpr static const auto GoogleLocale = "google_locale";
    constexpr static const auto GoogleLocaleDefault = "en";

    // Proxy settings for search suggestions
    constexpr static const auto Proxy = "proxy";
    constexpr static const auto ProxyDisabled = "disabled";
    constexpr static const auto ProxyHTTP = "http";
    constexpr static const auto ProxySocks5 = "socks5";
    constexpr static const auto ProxyShowErrors = "proxy_show_errors";

    // History cleaning options
    constexpr static const auto CleanHistory = "clean_history";
    constexpr static const auto CleanHistoryAll = "all";
    constexpr static const auto CleanHistoryQuick = "quick";
    constexpr static const auto CleanHistoryNone = "false";
    constexpr static const auto CleanHistoryDefault = CleanHistoryQuick;
};

const int Config::MinimumLetterCountDefault = 3; // NOLINT
const int Config::MaxSuggestionResultsDefault = 10; // NOLINT

struct SearchEngineConfig {
    constexpr static const auto Name = "name";
    constexpr static const auto Url = "url";
    constexpr static const auto Icon = "icon";
    constexpr static const auto OriginalName = "original_name";
};

#ifndef NO_PROXY_INTEGRATION
struct KWalletConfig {
    constexpr static const auto ProxyHostname = "Krunner-QuickWebShortcuts-proxy_hostname";
    constexpr static const auto ProxyPort = "Krunner-QuickWebShortcuts-proxy_port";
    constexpr static const auto ProxyUsername = "Krunner-QuickWebShortcuts-proxy_username";
    constexpr static const auto ProxyPassword = "Krunner-QuickWebShortcuts-proxy_password";
};
#endif
#endif // QUICKWEBSHORTCUTS_CONFIG_H
