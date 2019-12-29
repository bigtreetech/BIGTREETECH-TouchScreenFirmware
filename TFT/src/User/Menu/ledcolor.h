#ifndef _LEDCOLOR_H_
#define _LEDCPLOR_H_

#include "variants.h"

//24bit                           // G  R  B
#define LED_OFF                   0x00000000                                  
#define LED_WHITE                 0x00FFFFFF
#define LED_RED                   0x0000FF00
#define LED_ORANGE                0x00FF8C00
#define LED_YELLOW                0x00FFFF00
#define LED_GREEN                 0x00FF0000
#define LED_BLUE                  0x000000FF  
#define LED_INDIGO                0x004B0082D
#define LED_VIOLET                0x0000FEFE



/*颜色宏定义*/								/*R G B*/
#define	COLOR_LIGHTPINK						0xFFB6C1	//	浅粉红
#define	COLOR_PINK							0xFFC0CB	//	粉红
#define	COLOR_CRIMSON						0xDC143C	//	猩红
#define	COLOR_LAVENDERBLUSH					0xFFF0F5	//	脸红的淡紫色
#define	COLOR_PALEVIOLETRED					0xDB7093	//	苍白的紫罗兰红色
#define	COLOR_HOTPINK						0xFF69B4	//	热情的粉红
#define	COLOR_DEEPPINK						0xFF1493	//	深粉色
#define	COLOR_MEDIUMVIOLETRED				0xC71585	//	适中的紫罗兰红色
#define	COLOR_ORCHID						0xDA70D6	//	兰花的紫色
#define	COLOR_THISTLE						0xD8BFD8	//	蓟
#define	COLOR_PLUM							0xDDA0DD	//	李子
#define	COLOR_VIOLET						0xEE82EE	//	紫罗兰
#define	COLOR_MAGENTA						0xFF00FF	//	洋红
#define	COLOR_FUCHSIA						0xFF00FF	//	灯笼海棠(紫红色)
#define	COLOR_DARKMAGENTA					0x8B008B	//	深洋红色
#define	COLOR_PURPLE						0x800080	//	紫色
#define	COLOR_MEDIUMORCHID					0xBA55D3	//	适中的兰花紫
#define	COLOR_DARKVOILET					0x9400D3	//	深紫罗兰色
#define	COLOR_DARKORCHID					0x9932CC	//	深兰花紫
#define	COLOR_INDIGO						0x4B0082	//	靛青
#define	COLOR_BLUEVIOLET					0x8A2BE2	//	深紫罗兰的蓝色
#define	COLOR_MEDIUMPURPLE					0x9370DB	//	适中的紫色
#define	COLOR_MEDIUMSLATEBLUE				0x7B68EE	//	适中的板岩暗蓝灰色
#define	COLOR_SLATEBLUE						0x6A5ACD	//	板岩暗蓝灰色
#define	COLOR_DARKSLATEBLUE					0x483D8B	//	深岩暗蓝灰色
#define	COLOR_LAVENDER						0xE6E6FA	//	熏衣草花的淡紫色
#define	COLOR_GHOSTWHITE					0xF8F8FF	//	幽灵的白色
#define	COLOR_BLUE							0x0000FF	//	纯蓝
#define	COLOR_MEDIUMBLUE					0x0000CD	//	适中的蓝色
#define	COLOR_MIDNIGHTBLUE					0x191970	//	午夜的蓝色
#define	COLOR_DARKBLUE						0x00008B	//	深蓝色
#define	COLOR_NAVY							0x000080	//	海军蓝
#define	COLOR_ROYALBLUE						0x4169E1	//	皇家蓝
#define	COLOR_CORNFLOWERBLUE				0x6495ED	//	矢车菊的蓝色
#define	COLOR_LIGHTSTEELBLUE				0xB0C4DE	//	淡钢蓝
#define	COLOR_LIGHTSLATEGRAY				0x778899	//	浅石板灰
#define	COLOR_SLATEGRAY						0x708090	//	石板灰
#define	COLOR_DODERBLUE						0x1E90FF	//	道奇蓝
#define	COLOR_ALICEBLUE						0xF0F8FF	//	爱丽丝蓝
#define	COLOR_STEELBLUE						0x4682B4	//	钢蓝
#define	COLOR_LIGHTSKYBLUE					0x87CEFA	//	淡蓝色
#define	COLOR_SKYBLUE						0x87CEEB	//	天蓝色
#define	COLOR_DEEPSKYBLUE					0x00BFFF	//	深天蓝
#define	COLOR_LIGHTBLUE						0xADD8E6	//	淡蓝
#define	COLOR_POWDERBLUE					0xB0E0E6	//	火药蓝
#define	COLOR_CADETBLUE						0x5F9EA0	//	军校蓝
#define	COLOR_AZURE							0xF0FFFF	//	蔚蓝色
#define	COLOR_LIGHTCYAN						0xE1FFFF	//	淡青色
#define	COLOR_PALETURQUOISE					0xAFEEEE	//	苍白的绿宝石
#define	COLOR_CYAN							0x00FFFF	//	青色
#define	COLOR_AQUA							0x00FFFF	//	水绿色
#define	COLOR_DARKTURQUOISE					0x00CED1	//	深绿宝石
#define	COLOR_DARKSLATEGRAY					0x2F4F4F	//	深石板灰
#define	COLOR_DARKCYAN						0x008B8B	//	深青色
#define	COLOR_TEAL							0x008080	//	水鸭色
#define	COLOR_MEDIUMTURQUOISE				0x48D1CC	//	适中的绿宝石
#define	COLOR_LIGHTSEAGREEN					0x20B2AA	//	浅海洋绿
#define	COLOR_TURQUOISE						0x40E0D0	//	绿宝石
#define	COLOR_AUQAMARIN						0x7FFFAA	//	绿玉\碧绿色
#define	COLOR_MEDIUMAQUAMARINE				0x00FA9A	//	适中的碧绿色
#define	COLOR_MEDIUMSPRINGGREEN				0x00FF7F	//	适中的春天的绿色
#define	COLOR_MINTCREAM						0xF5FFFA	//	薄荷奶油
#define	COLOR_SPRINGGREEN					0x3CB371	//	春天的绿色
#define	COLOR_SEAGREEN						0x2E8B57	//	海洋绿
#define	COLOR_HONEYDEW						0xF0FFF0	//	蜂蜜
#define	COLOR_LIGHTGREEN					0x90EE90	//	淡绿色
#define	COLOR_PALEGREEN						0x98FB98	//	苍白的绿色
#define	COLOR_DARKSEAGREEN					0x8FBC8F	//	深海洋绿
#define	COLOR_LIMEGREEN						0x32CD32	//	酸橙绿
#define	COLOR_LIME							0x00FF00	//	酸橙色
#define	COLOR_FORESTGREEN					0x228B22	//	森林绿
#define	COLOR_GREEN							0x008000	//	纯绿
#define	COLOR_DARKGREEN						0x006400	//	深绿色
#define	COLOR_CHARTREUSE					0x7FFF00	//	查特酒绿
#define	COLOR_LAWNGREEN						0x7CFC00	//	草坪绿
#define	COLOR_GREENYELLOW					0xADFF2F	//	绿黄色
#define	COLOR_OLIVEDRAB						0x556B2F	//	橄榄土褐色
#define	COLOR_BEIGE							0xF5F5DC	//	米色(浅褐色)
#define	COLOR_LIGHTGOLDENRODYELLOW			0xFAFAD2	//	浅秋麒麟黄
#define	COLOR_IVORY							0xFFFFF0	//	象牙
#define	COLOR_LIGHTYELLOW					0xFFFFE0	//	浅黄色
#define	COLOR_YELLOW						0xFFFF00	//	纯黄
#define	COLOR_OLIVE							0x808000	//	橄榄
#define	COLOR_DARKKHAKI						0xBDB76B	//	深卡其布
#define	COLOR_LEMONCHIFFON					0xFFFACD	//	柠檬薄纱
#define	COLOR_PALEGODENROD					0xEEE8AA	//	灰秋麒麟
#define	COLOR_KHAKI							0xF0E68C	//	卡其布
#define	COLOR_GOLD							0xFFD700	//	金
#define	COLOR_CORNISLK						0xFFF8DC	//	玉米色
#define	COLOR_GOLDENROD						0xDAA520	//	秋麒麟
#define	COLOR_FLORALWHITE					0xFFFAF0	//	花的白色
#define	COLOR_OLDLACE						0xFDF5E6	//	老饰带
#define	COLOR_WHEAT							0xF5DEB3	//	小麦色
#define	COLOR_MOCCASIN						0xFFE4B5	//	鹿皮鞋
#define	COLOR_ORANGE						0xFFA500	//	橙色
#define	COLOR_PAPAYAWHIP					0xFFEFD5	//	番木瓜
#define	COLOR_BLANCHEDALMOND				0xFFEBCD	//	漂白的杏仁
#define	COLOR_NAVAJOWHITE					0xFFDEAD	//	纳瓦霍白
#define	COLOR_ANTIQUEWHITE					0xFAEBD7	//	古代的白色
#define	COLOR_TAN							0xD2B48C	//	晒黑
#define	COLOR_BRULYWOOD						0xDEB887	//	结实的树
#define	COLOR_BISQUE						0xFFE4C4	//	(浓汤)乳脂,番茄等
#define	COLOR_DARKORANGE					0xFF8C00	//	深橙色
#define	COLOR_LINEN							0xFAF0E6	//	亚麻布
#define	COLOR_PERU							0xCD853F	//	秘鲁
#define	COLOR_PEACHPUFF						0xFFDAB9	//	桃色
#define	COLOR_SANDYBROWN					0xF4A460	//	沙棕色
#define	COLOR_CHOCOLATE						0xD2691E	//	巧克力
#define	COLOR_SADDLEBROWN					0x8B4513	//	马鞍棕色
#define	COLOR_SEASHELL						0xFFF5EE	//	海贝壳
#define	COLOR_SIENNA						0xA0522D	//	黄土赭色
#define	COLOR_LIGHTSALMON					0xFFA07A	//	浅鲜肉(鲑鱼)色
#define	COLOR_CORAL							0xFF7F50	//	珊瑚
#define	COLOR_ORANGERED						0xFF4500	//	橙红色
#define	COLOR_DARKSALMON					0xE9967A	//	深鲜肉(鲑鱼)色
#define	COLOR_TOMATO						0xFF6347	//	番茄
#define	COLOR_MISTYROSE						0xFFE4E1	//	薄雾玫瑰
#define	COLOR_SALMON						0xFA8072	//	鲜肉(鲑鱼)色
#define	COLOR_SNOW							0xFFFAFA	//	雪
#define	COLOR_LIGHTCORAL					0xF08080	//	淡珊瑚色
#define	COLOR_ROSYBROWN						0xBC8F8F	//	玫瑰棕色
#define	COLOR_INDIANRED						0xCD5C5C	//	印度红
#define	COLOR_RED							0xFF0000	//	纯红
#define	COLOR_BROWN							0xA52A2A	//	棕色
#define	COLOR_FIREBRICK						0xB22222	//	耐火砖
#define	COLOR_DARKRED						0x8B0000	//	深红色
#define	COLOR_MAROON						0x800000	//	栗色
#define	COLOR_WHITE							0xFFFFFF	//	纯白
#define	COLOR_WHITESMOKE					0xF5F5F5	//	白烟
#define	COLOR_GAINSBORO						0xDCDCDC	//	亮灰色
#define	COLOR_LIGHTGREY						0xD3D3D3	//	浅灰色
#define	COLOR_SILVER						0xC0C0C0	//	银白色
#define	COLOR_DARKGRAY						0xA9A9A9	//	深灰色
#define	COLOR_GRAY							0x808080	//	灰色
#define	COLOR_DIMGRAY						0x696969	//	暗淡的灰色
#define	COLOR_BLACK							0x000000	//	纯黑


void led_color_Init(uint16_t psc,uint16_t arr); //
void LED_color_PIN_IPN(void);
void ws2812_send_DAT(uint32_t ws2812_dat);
#endif
