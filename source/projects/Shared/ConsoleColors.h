#pragma once

#ifndef _COLORS_
#define _COLORS_

/* FOREGROUND */
#define RST    std::string{"\x1B[0m"}
#define KRED   std::string{"\x1B[31m"}
#define KGRN   std::string{"\x1B[32m"}
#define KYEL   std::string{"\x1B[33m"}
#define KBLU   std::string{"\x1B[34m"}
#define KMAG   std::string{"\x1B[35m"}
#define KCYN   std::string{"\x1B[36m"}
#define KWHT   std::string{"\x1B[37m"}

#define FRED(x) KRED + x + RST
#define FGRN(x) KGRN + x + RST
#define FYEL(x) KYEL + x + RST
#define FBLU(x) KBLU + x + RST
#define FMAG(x) KMAG + x + RST
#define FCYN(x) KCYN + x + RST
#define FWHT(x) KWHT + x + RST

#define BOLD(x)  (std::string{"\x1B[1m"} + (x) + RST)
#define UNDL(x)  (std::string{"\x1B[4m"} + (x) + RST)

#endif  /* _COLORS_ */