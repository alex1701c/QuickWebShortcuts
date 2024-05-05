#ifndef QUICKWEBSHORTCUTS_API_LANGUAGE_UTILITY_H
#define QUICKWEBSHORTCUTS_API_LANGUAGE_UTILITY_H

#include <QComboBox>
// Shorter syntax for macro
#define s(str) QStringLiteral(str)

inline void setBingLanguages(QComboBox *box)
{
    box->clear();
    box->addItem(s("ar-sa (Arabic)"), s("ar-sa"));
    box->addItem(s("da-dk (Danish)"), s("da-dk"));
    box->addItem(s("de-at (German-Austria)"), s("de-at"));
    box->addItem(s("de-ch (German-Switzerland)"), s("de-ch"));
    box->addItem(s("de-de (German-Germany)"), s("de-de"));
    box->addItem(s("en-au (English-Australia)"), s("en-au"));
    box->addItem(s("en-ca (English-Canada)"), s("en-ca"));
    box->addItem(s("en-gb (English-UK)"), s("en-gb"));
    box->addItem(s("en-id (English-Indonesia)"), s("en-id"));
    box->addItem(s("en-ie (English-Ireland)"), s("en-ie"));
    box->addItem(s("en-in (English-India)"), s("en-in"));
    box->addItem(s("en-my (English-Malaysia)"), s("en-my"));
    box->addItem(s("en-mx (English-Mexico)"), s("en-mx"));
    box->addItem(s("en-nz (English-New Zealand)"), s("en-nz"));
    box->addItem(s("en-ph (English-Philippines)"), s("en-ph"));
    box->addItem(s("en-us (English-United States)"), s("en-us"));
    box->addItem(s("en-za (English-South Africa)"), s("en-za"));
    box->addItem(s("es-ar (Spanish-Argentina)"), s("es-ar"));
    box->addItem(s("es-cl (Spanish-Chile)"), s("es-cl"));
    box->addItem(s("es-mx (Spanish-Mexico)"), s("es-mx"));
    box->addItem(s("es-es (Spanish-Spain)"), s("es-es"));
    box->addItem(s("es-us (Spanish-United States)"), s("es-us"));
    box->addItem(s("fi-fi (Finnish)"), s("fi-fi"));
    box->addItem(s("fr-be (French-Belgium)"), s("fr-be"));
    box->addItem(s("fr-ca (French-Canada)"), s("fr-ca"));
    box->addItem(s("fr-ch (French-Switzerland)"), s("fr-ch"));
    box->addItem(s("fr-fr (French-France)"), s("fr-fr"));
    box->addItem(s("it-it (Italian)"), s("it-it"));
    box->addItem(s("ja-jp (Japanese)"), s("ja-jp"));
    box->addItem(s("ko-kr (Korean)"), s("ko-kr"));
    box->addItem(s("nl-be (Dutch-Belgium)"), s("nl-be"));
    box->addItem(s("nl-nl (Dutch_Netherlands)"), s("nl-nl"));
    box->addItem(s("no-no (Norwegian)"), s("no-no"));
    box->addItem(s("pl-pl (Polish)"), s("pl-pl"));
    box->addItem(s("pt-pt (Portuguese-Portugal)"), s("pt-pt"));
    box->addItem(s("pt-br (Portuguese-Brazil)"), s("pt-br"));
    box->addItem(s("ru-ru (Russian)"), s("ru-ru"));
    box->addItem(s("sv-se (Swedish)"), s("sv-se"));
    box->addItem(s("tr-tr (Turkish)"), s("tr-tr"));
    box->addItem(s("zh-cn (Chinese)"), s("zh-cn"));
    box->addItem(s("zh-hk (Traditional Chinese-Hong Kong SAR)"), s("zh-hk"));
    box->addItem(s("zh-tw (Traditional Chinese-Taiwan)"), s("zh-tw"));
}

inline void setGoogleLanguages(QComboBox *box)
{
    box->clear();
    // https://sites.google.com/site/tomihasa/google-language-codes
    box->addItem(s("Afrikaans"), s("af"));
    box->addItem(s("Akan"), s("ak"));
    box->addItem(s("Albanian"), s("sq"));
    box->addItem(s("Amharic"), s("am"));
    box->addItem(s("Arabic"), s("ar"));
    box->addItem(s("Armenian"), s("hy"));
    box->addItem(s("Azerbaijani"), s("az"));
    box->addItem(s("Basque"), s("eu"));
    box->addItem(s("Belarusian"), s("be"));
    box->addItem(s("Bemba"), s("bem"));
    box->addItem(s("Bengali"), s("bn"));
    box->addItem(s("Bihari"), s("bh"));
    box->addItem(s("Bork, bork, bork!"), s("xx-bork"));
    box->addItem(s("Bosnian"), s("bs"));
    box->addItem(s("Breton"), s("br"));
    box->addItem(s("Bulgarian"), s("bg"));
    box->addItem(s("Cambodian"), s("km"));
    box->addItem(s("Catalan"), s("ca"));
    box->addItem(s("Cherokee"), s("chr"));
    box->addItem(s("Chichewa"), s("ny"));
    box->addItem(s("Chinese (Simplified)"), s("zh-CN"));
    box->addItem(s("Chinese (Traditional)"), s("zh-TW"));
    box->addItem(s("Corsican"), s("co"));
    box->addItem(s("Croatian"), s("hr"));
    box->addItem(s("Czech"), s("cs"));
    box->addItem(s("Danish"), s("da"));
    box->addItem(s("Dutch"), s("nl"));
    box->addItem(s("Elmer Fudd"), s("xx-elmer"));
    box->addItem(s("English"), s("en"));
    box->addItem(s("Esperanto"), s("eo"));
    box->addItem(s("Estonian"), s("et"));
    box->addItem(s("Ewe"), s("ee"));
    box->addItem(s("Faroese"), s("fo"));
    box->addItem(s("Filipino"), s("tl"));
    box->addItem(s("Finnish"), s("fi"));
    box->addItem(s("French"), s("fr"));
    box->addItem(s("Frisian"), s("fy"));
    box->addItem(s("Ga"), s("gaa"));
    box->addItem(s("Galician"), s("gl"));
    box->addItem(s("Georgian"), s("ka"));
    box->addItem(s("German"), s("de"));
    box->addItem(s("Greek"), s("el"));
    box->addItem(s("Guarani"), s("gn"));
    box->addItem(s("Gujarati"), s("gu"));
    box->addItem(s("Hacker"), s("xx-hacker"));
    box->addItem(s("Haitian Creole"), s("ht"));
    box->addItem(s("Hausa"), s("ha"));
    box->addItem(s("Hawaiian"), s("haw"));
    box->addItem(s("Hebrew"), s("iw"));
    box->addItem(s("Hindi"), s("hi"));
    box->addItem(s("Hungarian"), s("hu"));
    box->addItem(s("Icelandic"), s("is"));
    box->addItem(s("Igbo"), s("ig"));
    box->addItem(s("Indonesian"), s("id"));
    box->addItem(s("Interlingua"), s("ia"));
    box->addItem(s("Irish"), s("ga"));
    box->addItem(s("Italian"), s("it"));
    box->addItem(s("Japanese"), s("ja"));
    box->addItem(s("Javanese"), s("jw"));
    box->addItem(s("Kannada"), s("kn"));
    box->addItem(s("Kazakh"), s("kk"));
    box->addItem(s("Kinyarwanda"), s("rw"));
    box->addItem(s("Kirundi"), s("rn"));
    box->addItem(s("Klingon"), s("xx-klingon"));
    box->addItem(s("Kongo"), s("kg"));
    box->addItem(s("Korean"), s("ko"));
    box->addItem(s("Krio (Sierra Leone)"), s("kri"));
    box->addItem(s("Kurdish"), s("ku"));
    box->addItem(s("Kurdish (Soranî)"), s("ckb"));
    box->addItem(s("Kyrgyz"), s("ky"));
    box->addItem(s("Laothian"), s("lo"));
    box->addItem(s("Latin"), s("la"));
    box->addItem(s("Latvian"), s("lv"));
    box->addItem(s("Lingala"), s("ln"));
    box->addItem(s("Lithuanian"), s("lt"));
    box->addItem(s("Lozi"), s("loz"));
    box->addItem(s("Luganda"), s("lg"));
    box->addItem(s("Luo"), s("ach"));
    box->addItem(s("Macedonian"), s("mk"));
    box->addItem(s("Malagasy"), s("mg"));
    box->addItem(s("Malay"), s("ms"));
    box->addItem(s("Malayalam"), s("ml"));
    box->addItem(s("Maltese"), s("mt"));
    box->addItem(s("Maori"), s("mi"));
    box->addItem(s("Marathi"), s("mr"));
    box->addItem(s("Mauritian Creole"), s("mfe"));
    box->addItem(s("Moldavian"), s("mo"));
    box->addItem(s("Mongolian"), s("mn"));
    box->addItem(s("Montenegrin"), s("sr-ME"));
    box->addItem(s("Nepali"), s("ne"));
    box->addItem(s("Nigerian Pidgin"), s("pcm"));
    box->addItem(s("Northern Sotho"), s("nso"));
    box->addItem(s("Norwegian"), s("no"));
    box->addItem(s("Norwegian (Nynorsk)"), s("nn"));
    box->addItem(s("Occitan"), s("oc"));
    box->addItem(s("Oriya"), s("or"));
    box->addItem(s("Oromo"), s("om"));
    box->addItem(s("Pashto"), s("ps"));
    box->addItem(s("Persian"), s("fa"));
    box->addItem(s("Pirate"), s("xx-pirate"));
    box->addItem(s("Polish"), s("pl"));
    box->addItem(s("Portuguese (Brazil)"), s("pt-BR"));
    box->addItem(s("Portuguese (Portugal)"), s("pt-PT"));
    box->addItem(s("Punjabi"), s("pa"));
    box->addItem(s("Quechua"), s("qu"));
    box->addItem(s("Romanian"), s("ro"));
    box->addItem(s("Romansh"), s("rm"));
    box->addItem(s("Runyakitara"), s("nyn"));
    box->addItem(s("Russian"), s("ru"));
    box->addItem(s("Scots Gaelic"), s("gd"));
    box->addItem(s("Serbian"), s("sr"));
    box->addItem(s("Serbo-Croatian"), s("sh"));
    box->addItem(s("Sesotho"), s("st"));
    box->addItem(s("Setswana"), s("tn"));
    box->addItem(s("Seychellois Creole"), s("crs"));
    box->addItem(s("Shona"), s("sn"));
    box->addItem(s("Sindhi"), s("sd"));
    box->addItem(s("Sinhalese"), s("si"));
    box->addItem(s("Slovak"), s("sk"));
    box->addItem(s("Slovenian"), s("sl"));
    box->addItem(s("Somali"), s("so"));
    box->addItem(s("Spanish"), s("es"));
    box->addItem(s("Spanish (Latin American)"), s("es-419"));
    box->addItem(s("Sundanese"), s("su"));
    box->addItem(s("Swahili"), s("sw"));
    box->addItem(s("Swedish"), s("sv"));
    box->addItem(s("Tajik"), s("tg"));
    box->addItem(s("Tamil"), s("ta"));
    box->addItem(s("Tatar"), s("tt"));
    box->addItem(s("Telugu"), s("te"));
    box->addItem(s("Thai"), s("th"));
    box->addItem(s("Tigrinya"), s("ti"));
    box->addItem(s("Tonga"), s("to"));
    box->addItem(s("Tshiluba"), s("lua"));
    box->addItem(s("Tumbuka"), s("tum"));
    box->addItem(s("Turkish"), s("tr"));
    box->addItem(s("Turkmen"), s("tk"));
    box->addItem(s("Twi"), s("tw"));
    box->addItem(s("Uighur"), s("ug"));
    box->addItem(s("Ukrainian"), s("uk"));
    box->addItem(s("Urdu"), s("ur"));
    box->addItem(s("Uzbek"), s("uz"));
    box->addItem(s("Vietnamese"), s("vi"));
    box->addItem(s("Welsh"), s("cy"));
    box->addItem(s("Wolof"), s("wo"));
    box->addItem(s("Xhosa"), s("xh"));
    box->addItem(s("Yiddish"), s("yi"));
    box->addItem(s("Yoruba"), s("yo"));
    box->addItem(s("Zulu"), s("zu"));
}

#endif // QUICKWEBSHORTCUTS_API_LANGUAGE_UTILITY_H
