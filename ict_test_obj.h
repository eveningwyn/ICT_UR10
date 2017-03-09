#ifndef ICT_TEST_OBJ_H
#define ICT_TEST_OBJ_H

#include <QObject>

class ICT_Test_Obj : public QObject
{
    Q_OBJECT
public:
    explicit ICT_Test_Obj(QObject *parent = 0);

private:
    int ictPing();

signals:
    void ict_Error_Msg(QString errorMsg);

public slots:
    void getIctInfo(QString path);
    void setIctInfo(QString path);
};

#endif // ICT_TEST_OBJ_H
