#pragma once
//RU
//Garry`s Mod AIN ������/�������
//By anrej0705 under GPLv2

//EN
//Need translation help!
#include <fstream>
#include <vector>

//������, �������� �������� ����������� ���������
//������ ��������� AIN �� ������ 4, ������� ��� ����� ���������� ����� ��� ������
//��������� �� ������ ����� 4 �����, ���� ���� ���� ����� ������ � 1 ����
#ifdef _MSC_VER
#define PACKED_DATA( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif
#ifdef __GNUC__
#define PACKED_DATA( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif
//������ �������������
//PACKED_DATA(��������� ��� �����, ������� ����� ��������� � ������);

#define NUM_HULLS 10    //10 �� ���������, �������� �������� ������������ ���������

//����� �� Source SDK 2013
enum NodeType_enum
{
    NODE_ANY,			//������������ ��� ������� ���� ����
    NODE_DELETED,		//������������ �������� wc_edit ��� ������� ���� ��� ��������� �� ����   
    NODE_GROUND,        //�������� ���
    NODE_AIR,           //����������� ���
    NODE_CLIMB,         //����������� �� ������� ����� ����������
    NODE_WATER          //�������� �����, ��� ��� ������ �������
};
//����� �� Source SDK 2013
enum AI_ZoneIds
{
    AI_NODE_ZONE_UNKNOWN = 0,
    AI_NODE_ZONE_SOLO = 1,
    AI_NODE_ZONE_UNIVERSAL = 3,
    AI_NODE_FIRST_ZONE = 4,
};

//���������� �����
int numNodes;

//���������� ���������� ����������
int numLinks;

//������� ��������� ��� �������� ��������� � 3-������ ������������
struct Vector3
{
    float x;
    float y;
    float z;
};

//��������� ����� AIN
struct ain_header
{
    int ainet_ver;  //������ �����(37 ��� GMOD)
    int map_ver;    //������ �����(1765 ��� GMOD)
};

//��������� ����
PACKED_DATA(struct ain_node     //������ 61 ����
{
    Vector3 pos;                //[0 +12]x, y, z
    float yaw;                  //[12+4 ]������ ������ ��������� ��� ��
    float flOffsets[NUM_HULLS]; //[16+40]������������ �������� ��� ������� ���� ��������, ����� �������� 0
    uint8_t nodeType;           //[56+1 ]��� ����(������������ ����� ������������� NodeType_enum)
    uint16_t nodeInfoBits;      //[57+2 ]���� ��������� ����
    uint8_t zone;               //[59+1 ]??? AI_ZoneIds ???, �� ��������� 0
    uint8_t next;               //[60+1 ]������ ������ ��� ��������?
});

//�������� � ���������� �����������
PACKED_DATA(struct ain_link     //������ 40 ����
{                       
    int16_t srcId;              //����� ����, ������� ����� �������
    int16_t desId;              //����� ����, � ������� ��������������
    uint8_t moves[NUM_HULLS];   //��� ������� ��� 1, ��������� ������� ��� ����������� ���� ��������(1 - ��������, 0 - �� ��������)
});

//������� AIN ������. �������� �� gm_construct(�� ������ ������ gm_construct(excel 97).ain.xls)
class CAinFileDecoder
{
    private:
        std::ifstream* ain;
        ain_header* ain_hed;
        ain_node* node;
        ain_link* link;
        int lut_int;
    public:
        std::vector<ain_node> *node_list;   //������� �����
        std::vector<ain_link> *link_list;   //������� ���������� ����������
        std::vector<int>* lut;              //�������, ���������� ��� ������ �����, ������� ���� � ������� �����
    public:
        //��� ����� ����� �������� ���������, ������ � ����, ������
        //CAinFileDecoder("C:\git\rubyrose_ai_test.AIN");
        CAinFileDecoder(const char* file)  //MSVC C26495 - ��������, �� ��������
        {
            //���� ����� ������ - �������
            if (!file)
                return;

            //��������� ����
            ain = new std::ifstream(file, std::ios::binary);

            if (!ain->is_open())
                return; //���� ���� �� ���������� �� �������

            //���������� ��������� ���������
            ain_hed = new ain_header;
            ain->read((char*)ain_hed, sizeof(ain_header));

            if (ain_hed->ainet_ver < 17 || ain_hed->map_ver < 1765)
                return; //���� ������ ���� ���������� �� �������

            //������ ������
            node = new ain_node;
            link = new ain_link;

            //������ �������
            node_list = new std::vector<ain_node>;
            link_list = new std::vector<ain_link>;
            lut = new std::vector<int>;

            //���������� ��������� ������� �����
            ain->read((char*)&numNodes, sizeof(int));

            //���������� ��������� ���� ������ �� ����������� �� ����������
            for (uint16_t node_it = 0; node_it < numNodes; ++node_it)
            {
                ain->read((char*)node, sizeof(ain_node));
                node_list->push_back(*node);
            }

            //������ ��������� ������� ����������� �����
            ain->read((char*)&numLinks, sizeof(int));

            //�� ����������� �������� ��������� ����
            for (uint16_t link_it = 0; link_it < numLinks; ++link_it)
            {
                ain->read((char*)link, sizeof(ain_link));
                link_list->push_back(*link);
            }

            //������ ������ ��������� - ���?
            for (uint16_t lut_it = 0; lut_it < numNodes; ++lut_it)
            {
                ain->read((char*)&lut_int, sizeof(int));
                lut->push_back(lut_int);
            }

            //��������� � ������ �� �����
            ain->close();
            delete(ain);
            delete(node);
            delete(link);
        }
        //������� �� ����� ������
        ~CAinFileDecoder()
        {
            delete(node_list);
            delete(link_list);
            delete(lut);
        }
};
