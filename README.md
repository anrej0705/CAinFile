Инструментарий для работы с форматом AIN от Valve(Valve SDK, GMOD, TF2 и проч.)

Простой пример работы(большего и не нужно!)
```
#include "CAinFileDecoder.h"
int main()
{
    CAinFileDecoder ain("gm_construct.ain");  //Вся работа с AIN осуществляется в процессе создания объекта

    //Подведение итогов:
    std::vector<ain_node> n_list(*ain.node_list);
    std::vector<ain_link> l_list(*ain.link_list);
    std::vector<int> lut_list(*ain.lut);
}
```

***Need help with EN comments trasnalation***!
