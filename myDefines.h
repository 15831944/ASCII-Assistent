#ifndef MYDEFINES_H
#define MYDEFINES_H


#define INIFILE             "ascii_assistent.ini"
#define WERKZEUGDATEI       "tools.csv"


#define ASCII   ".ascii"
#define GANX    ".ganx"

#define ENDE_EINTRAG        ";"

//--------------------------------------Bearbeitungen:
#define BEARBEITUNG_BOHRUNG     "H"
#define BEARBEITUNG_NUT         "S"
#define BEARBEITUNG_FRAES       "M"


//--------------------------------------Bezüge der Bearbeitungen in der ASCII-Datei:
#define BEZUG_FLAECHE_OBEN_ASCII      " TOP"
#define BEZUG_FLAECHE_UNTEN_ASCII     " BOT"
#define BEZUG_FLAECHE_LINKS_ASCII     " LEF"
#define BEZUG_FLAECHE_RECHTS_ASCII    " RIG"
#define BEZUG_FLAECHE_VORNE_ASCII     " FRO"
#define BEZUG_FLAECHE_HINTEN_ASCII    " REA"






//--------------------------------------Bezüge der Bearbeitungen in der ganx-Datei:
#define BEZUG_FLAECHE_OBEN_GANX      ""
#define BEZUG_FLAECHE_UNTEN_GANX     "Bottom"
#define BEZUG_FLAECHE_LINKS_GANX     ""
#define BEZUG_FLAECHE_RECHTS_GANX    ""
#define BEZUG_FLAECHE_VORNE_GANX     ""
#define BEZUG_FLAECHE_HINTEN_GANX    ""

#define BEZUG_REF_OBEN_LINKS        "TL"
#define BEZUG_REF_OBEN_RECHTS       "TR"
#define BEZUG_REF_UNTEN_LINKS       "BL"
#define BEZUG_REF_UNTEN_RECHTS      "BR"


//--------------------------------------Werkzeug:
#define WKZ_TYP_BOHRER              "B"
#define WKZ_TYP_SAEGE               "S"
#define WKZ_TYP_FRAESER             "M"

#endif // MYDEFINES_H
