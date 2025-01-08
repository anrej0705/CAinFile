#pragma once
//RU
//Garry`s Mod AIN парсер/декодер
//By anrej0705 under GPLv2

//EN
//Need translation help!
#include <fstream>
#include <vector>

//Макрос, задающий аттрибут упакованной структуры
//размер структуры AIN не кратен 4, поэтому его нужно запаковать чтобы при чтении
//указатель не прыгал через 4 байта, даже если поле имеет размер в 1 байт
#ifdef _MSC_VER
#define PACKED_DATA( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif
#ifdef __GNUC__
#define PACKED_DATA( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif
//Пример использования
//PACKED_DATA(структура или класс, который нужно упаковать в памяти);

#define NUM_HULLS 10    //10 по умолчанию, вероятно числовое разнообразие сущностей

//Взято из Source SDK 2013
enum NodeType_enum
{
    NODE_ANY,			//Используется для задания типа узла
    NODE_DELETED,		//Используется командой wc_edit для пометки узла как удалённого на ходу   
    NODE_GROUND,        //Наземный тип
    NODE_AIR,           //Подвешенный тип
    NODE_CLIMB,         //Поверхность по которой можно взобраться
    NODE_WATER          //Сообщает ботам, что они должны плавать
};
//Взято из Source SDK 2013
enum AI_ZoneIds
{
    AI_NODE_ZONE_UNKNOWN = 0,
    AI_NODE_ZONE_SOLO = 1,
    AI_NODE_ZONE_UNIVERSAL = 3,
    AI_NODE_FIRST_ZONE = 4,
};

//Количество узлов
int numNodes;

//Количество межузловых соединений
int numLinks;

//Простая структура для хранения координат в 3-мерном пространстве
struct Vector3
{
    float x;
    float y;
    float z;
};

//Заголовок карты AIN
struct ain_header
{
    int ainet_ver;  //Версия файла(37 для GMOD)
    int map_ver;    //Версия карты(1765 для GMOD)
};

//Заголовок узла
PACKED_DATA(struct ain_node     //Размер 61 байт
{
    Vector3 pos;                //[0 +12]x, y, z
    float yaw;                  //[12+4 ]Радиус поиска подсказок для ИИ
    float flOffsets[NUM_HULLS]; //[16+40]Вертикальное смещение для каждого типа сущности, можно оставить 0
    uint8_t nodeType;           //[56+1 ]Тип узла(определяется через перечислитель NodeType_enum)
    uint16_t nodeInfoBits;      //[57+2 ]Биты настройки ноды
    uint8_t zone;               //[59+1 ]??? AI_ZoneIds ???, по умолчанию 0
    uint8_t next;               //[60+1 ]Первая ссылка для проверки?
});

//Сведение о межузловых соединениях
PACKED_DATA(struct ain_link     //Размер 40 байт
{                       
    int16_t srcId;              //Номер узла, который будет соединён
    int16_t desId;              //Номер узла, к которой присоединяется
    uint8_t moves[NUM_HULLS];   //Как правило тут 1, разрешает переход для конкретного типа сущности(1 - разрешён, 0 - не разрешён)
});

//Декодер AIN файлов. Проверен на gm_construct(см пример работы gm_construct(excel 97).ain.xls)
class CAinFileDecoder
{
    private:
        std::ifstream* ain = NULL;
        ain_header* ain_hed = NULL;
        ain_node* node = NULL;
        ain_link* link = NULL;
        int lut_int = 0;
    public:
        std::vector<ain_node> *node_list = NULL;   //Таблица узлов
        std::vector<ain_link> *link_list = NULL;   //Таблица межузловых соединений
        std::vector<int>* lut = NULL;              //Таблица, содержащая все номера узлов, которые есть в текущем файле
    public:
        //Имя карты нужно передать полностью, вместе с путём, пример
        //CAinFileDecoder("C:\git\rubyrose_ai_test.AIN");
        CAinFileDecoder(const char* file)
        {
            //Если вызов пустой - выходим
            if (!file)
                return;

            //Открываем файл
            ain = new std::ifstream(file, std::ios::binary);

            if (!ain->is_open())
                return; //Если файл не существует то выходим

            //Попытаемся прочитать заголовок
            ain_hed = new ain_header;
            ain->read((char*)ain_hed, sizeof(ain_header));

            if (ain_hed->ainet_ver < 37 || ain_hed->map_ver < 17)
                return; //Если версия ниже гмодовской то выходим

            //Создаём буфера
            node = new ain_node;
            link = new ain_link;

            //Создаём таблицы
            node_list = new std::vector<ain_node>;
            link_list = new std::vector<ain_link>;
            lut = new std::vector<int>;

            //Попытаемся сосчитать счётчик узлов
            ain->read((char*)&numNodes, sizeof(int));

            //Попытаемся прочитать узлы исходя из полученного их количества
            for (uint16_t node_it = 0; node_it < numNodes; ++node_it)
            {
                ain->read((char*)node, sizeof(ain_node));
                node_list->push_back(*node);
            }

            //Теперь сосчитаем счётчик соединенных узлов
            ain->read((char*)&numLinks, sizeof(int));

            //По полученному счётчику запускаем цикл
            for (uint16_t link_it = 0; link_it < numLinks; ++link_it)
            {
                ain->read((char*)link, sizeof(ain_link));
                link_list->push_back(*link);
            }

            //Теперь циклом считываем - что?
            for (uint16_t lut_it = 0; lut_it < numNodes; ++lut_it)
            {
                ain->read((char*)&lut_int, sizeof(int));
                lut->push_back(lut_int);
            }

            //Закрываем и чистим за собой
            ain->close();
            delete(ain);
            delete(node);
            delete(link);
        }
        //Чиститм за собой память
        ~CAinFileDecoder()
        {
            delete(node_list);
            delete(link_list);
            delete(lut);
        }
};
