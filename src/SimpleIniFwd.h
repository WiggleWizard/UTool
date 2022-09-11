#pragma once

template<class SI_CHAR> struct SI_NoCase;
template<class SI_CHAR> class SI_ConvertA;
template<class SI_CHAR, class SI_STRLESS, class SI_CONVERTER> class CSimpleIniTempl;
typedef CSimpleIniTempl<char, SI_NoCase<char>, SI_ConvertA<char>> CSimpleIniA;