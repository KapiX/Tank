#ifndef TANKMAPWIDGET_H
#define TANKMAPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDataStream>

// pierwszy bajt - x, kolejny - y
enum BLOCK_TYPE
{
    BT_EMPTY = 4120,
    BT_BRICK = 0,
    BT_BRICK_HBOT = 2048,
    BT_BRICK_HTOP = 2056,
    BT_BRICK_VLEFT = 4096,
    BT_BRICK_VRIGHT = 4104,
    BT_BRICK_LBOT = 6144,
    BT_BRICK_RTOP = 6152,
    BT_BRICK_RBOT = 6160,
    BT_BRICK_LTOP = 6168,
    BT_STEEL = 8,
    BT_SEA = 16,
    BT_JUNGLE = 2064,
    BT_ICE = 2072,
    BT_EAGLELTOP = 32,
    BT_EAGLELBOT = 40,
    BT_EAGLERTOP = 2080,
    BT_EAGLERBOT = 2088,
    BT_PLAYER1LTOP = 4128,
    BT_PLAYER1LBOT = 4136,
    BT_PLAYER1RTOP = 6176,
    BT_PLAYER1RBOT = 6184,
    BT_PLAYER2LTOP = 48,
    BT_PLAYER2LBOT = 56,
    BT_PLAYER2RTOP = 2096,
    BT_PLAYER2RBOT = 2104,
    BT_ENEMYLTOP = 4144,
    BT_ENEMYLBOT = 4152,
    BT_ENEMYRTOP = 6192,
    BT_ENEMYRBOT = 6200,
    BT_EDGE = 4112
};

enum TOOLS
{
    EMPTY = 0,
    BRICK,
    STEEL,
    SEA,
    JUNGLE,
    ICE,
    EAGLE,
    EDGE,
    P1,
    P2,
    E
};

struct TLV1_BonusRect
{
    quint16 x;
    quint16 y;
    quint16 w;
    quint16 h;
};

struct TLV1_Point
{
    qint8 x;
    qint8 y;
};

enum TLV1_BLOCK_TYPES
{
    TLV1_EMPTY = 0,
    TLV1_BRICK,
    TLV1_STEEL,
    TLV1_SEA,
    TLV1_JUNGLE,
    TLV1_ICE,
    TLV1_EDGE/*,
    TLV1_EAGLE_LT,
    TLV1_EAGLE_LB,
    TLV1_EAGLE_RT,
    TLV1_EAGLE_RB,
    TLV1_PLAYER1_LT,
    TLV1_PLAYER1_LB,
    TLV1_PLAYER1_RT,
    TLV1_PLAYER1_RB,
    TLV1_PLAYER2_LT,
    TLV1_PLAYER2_LB,
    TLV1_PLAYER2_RT,
    TLV1_PLAYER2_RB,
    TLV1_ENEMY_LT,
    TLV1_ENEMY_LB,
    TLV1_ENEMY_RT,
    TLV1_ENEMY_RB*/
};

struct TLV1_Header
{
    char id[4];
    quint8 nameLength;
    char *name;
    TLV1_BonusRect bonusRect;
    TLV1_Point p1;
    TLV1_Point p2;
    TLV1_Point e1;
    TLV1_Point e2;
    TLV1_Point e3;
    TLV1_Point eagle;
};

class TankMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TankMapWidget(QWidget *parent = 0);
    ~TankMapWidget();

    QSize sizeHint() const;

    bool getChanged() const { return changed; }

    TOOLS getSelectedTool() const { return selectedTool; }
    bool getPart(int x, int y) const { return parts[x][y]; }
    bool getDrawGrid() const { return drawGrid; }
    TLV1_BonusRect getBonusRect() const { return bRect; }
    QString getMapName() const { return mapName; }
    TLV1_Point getSpawnP1() const { return spawnP1; }
    TLV1_Point getSpawnP2() const { return spawnP2; }
    TLV1_Point getSpawnE1() const { return spawnE1; }
    TLV1_Point getSpawnE2() const { return spawnE2; }
    TLV1_Point getSpawnE3() const { return spawnE3; }
    TLV1_Point getEagle() const { return eagle; }

    static void setEmptyButton(QPushButton *btn) { btnEmpty = btn; }
    static void setEagleButton(QPushButton *btn) { btnEagle = btn; }
    static void setP1Button(QPushButton *btn) { btnP1 = btn; }
    static void setP2Button(QPushButton *btn) { btnP2 = btn; }
    static void setEButton(QPushButton *btn) { btnE = btn; }

signals:
    void partChanged(int x, int y, bool newB);
    void toolChanged(TOOLS newTool);
    void mapNameChanged(QString name);
    void mapChanged(int x, int y);

public slots:
    void setPart(int x, int y, bool b);
    void changeTool(TOOLS t);
    void setDrawGrid(bool b);
    void setBonusRect(quint16 x, quint16 y, quint16 w, quint16 h);
    void setMapName(QString name);

    void save();
    bool load(QString filename = QString());

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    bool changed;

    QPixmap *blocks;
    QPoint curPos;
    TOOLS selectedTool;
    bool parts[4][4];
    bool drawGrid;
    QLine vertLines[25];
    QLine horLines[17];
    TLV1_BonusRect bRect;
    QString mapName;
    TLV1_Point spawnP1;
    TLV1_Point spawnP2;
    TLV1_Point spawnE1;
    TLV1_Point spawnE2;
    TLV1_Point spawnE3;
    TLV1_Point eagle;

    BLOCK_TYPE map[50][34];

    static QPushButton *btnEmpty;
    static QPushButton *btnEagle;
    static QPushButton *btnP1;
    static QPushButton *btnP2;
    static QPushButton *btnE;

    void writeUInt8(QDataStream *stream, quint8 i);
    void writeUInt16(QDataStream *stream, quint16 i);

    void writeBonusRect(QDataStream *stream, TLV1_BonusRect br);
    void writePoint(QDataStream *stream, TLV1_Point p);

    void readUInt8(QDataStream *stream, quint8 *i);
    void readUInt16(QDataStream *stream, quint16 *i);

    void readBonusRect(QDataStream *stream, TLV1_BonusRect *br);
    void readPoint(QDataStream *stream, TLV1_Point *p);
};

#endif // TANKMAPWIDGET_H
