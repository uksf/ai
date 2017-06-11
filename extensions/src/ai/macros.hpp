//TEMP: not sure if I'll use these

#ifndef MAINPREFIX
    #define MAINPREFIX u
#endif

#ifndef PREFIX
    #define PREFIX uksf
#endif

#ifndef SUBPREFIX
    #define SUBPREFIX addons
#endif

#define DOUBLES(var1,var2) var1##_##var2
#define TRIPLES(var1,var2,var3) var1##_##var2##_##var3
#define QUOTE(var1) #var1
#define ADDON DOUBLES(PREFIX,COMPONENT)

#ifndef PATHTOF_SYS
    #define PATHTOF_SYS(var1,var2,var3) \\MAINPREFIX\\##var1\\SUBPREFIX\\##var2\\##var3
#endif
#ifndef PATHTOF2_SYS
    #define PATHTOF2_SYS(var1,var2,var3) MAINPREFIX\\##var1\\SUBPREFIX\\##var2\\##var3
#endif

#define PATHTO_R(var1) PATHTOF2_SYS(PREFIX,COMPONENT,var1)
#define QPATHTO_R(var1) QUOTE(PATHTO_R(var1))
#define PATHTOF(var1) PATHTOF_SYS(PREFIX,COMPONENT,var1)
#define PATHTOEF(var1,var2) PATHTOF_SYS(PREFIX,var1,var2)
#define QPATHTOF(var1) QUOTE(PATHTOF(var1))
#define QPATHTOEF(var1,var2) QUOTE(PATHTOEF(var1,var2))

#define GVAR(var1) DOUBLES(ADDON,var1)
#define EGVAR(var1,var2) TRIPLES(PREFIX,var1,var2)
#define QGVAR(var1) QUOTE(GVAR(var1))
#define QEGVAR(var1,var2) QUOTE(EGVAR(var1,var2))
#define QQGVAR(var1) QUOTE(QGVAR(var1))
#define QQEGVAR(var1,var2) QUOTE(QEGVAR(var1,var2))