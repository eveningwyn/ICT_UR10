#include "mescheckobj.h"
#include "language.h"

MesCheckObj::MesCheckObj(QObject *parent) : QObject(parent)
{

}

void MesCheckObj::init_mes()
{
    mesEnable = true;
}

void MesCheckObj::checkSn(QString sn)
{
    if(true == mesEnable)
    {
        if(true)
        {
            emit checkSnResult(sn,true);
        }
        else
        {
            emit checkSnResult(sn,false);
            emit mes_Error_Msg(tr("当前产品不属于当前工站，请确认该产品已正常过站！\n"));
        }
    }
    else
    {
        emit checkSnResult(sn,true);
    }
}

void MesCheckObj::set_mes_enable(bool enable)
{
    mesEnable = enable;
}
