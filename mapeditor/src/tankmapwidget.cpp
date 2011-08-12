#include "tankmapwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>

QPushButton *TankMapWidget::btnEmpty = NULL;
QPushButton *TankMapWidget::btnEagle = NULL;
QPushButton *TankMapWidget::btnP1 = NULL;
QPushButton *TankMapWidget::btnP2 = NULL;
QPushButton *TankMapWidget::btnE = NULL;

TankMapWidget::TankMapWidget(QWidget *parent) :
    QWidget(parent),
    changed(false),
    curPos(-1, -1),
    selectedTool(EMPTY),
    drawGrid(false),
    mapName("")
{
    setMouseTracking(true);
    bRect.x = 0;
    bRect.y = 0;
    bRect.w = 1;
    bRect.h = 1;
    blocks = new QPixmap(":/images/blocks.png");
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            parts[i][j] = true;
        }
    }
    for(int x = 0; x < 25; x++)
    {
        vertLines[x] = QLine(x * 32, 0, x * 32, 544);
    }
    for(int y = 0; y < 17; y++)
    {
        horLines[y] = QLine(0, y * 32, 800, y * 32);
    }
    for(int x = 0; x < 50; x++)
    {
        for(int y = 0; y < 34; y++)
        {
            map[x][y] = BT_EMPTY;
        }
    }
    spawnP1.x = spawnP1.y = -1;
    spawnP2.x = spawnP2.y = -1;
    spawnE1.x = spawnE1.y = -1;
    spawnE2.x = spawnE2.y = -1;
    spawnE3.x = spawnE3.y = -1;
    eagle.x = eagle.y = -1;
}

TankMapWidget::~TankMapWidget()
{
    delete blocks;
}

QSize TankMapWidget::sizeHint() const
{
    return QSize(800, 544);
}

void TankMapWidget::setPart(int x, int y, bool b)
{
    if(b != parts[x][y])
    {
        parts[x][y] = b;
        emit partChanged(x, y, b);
    }
}

void TankMapWidget::changeTool(TOOLS t)
{
    if(t != selectedTool)
    {
        selectedTool = t;
        emit toolChanged(t);
    }
}

void TankMapWidget::setDrawGrid(bool b)
{
    if(b != drawGrid)
    {
        drawGrid = b;
        repaint();
    }
}

void TankMapWidget::setBonusRect(quint16 x, quint16 y, quint16 w, quint16 h)
{
    if(bRect.x != x || bRect.y != y || bRect.w != w || bRect.h != h)
    {
        bRect.x = x;
        bRect.y = y;
        bRect.w = w;
        bRect.h = h;
        repaint();
    }
}

void TankMapWidget::setMapName(QString name)
{
    if(name != mapName)
    {
        mapName = name;
        emit mapNameChanged(name);
    }
}

void TankMapWidget::save()
{
    QString filename = QFileDialog::getSaveFileName(0, tr("Save"), QDir::currentPath(), tr("Tank level files (*.tlv)"));
    if(!filename.isNull())
    {
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly))
        {
            QDataStream stream(&file);

            // header
            TLV1_Header h;
            h.id[0] = 'T';
            h.id[1] = 'L';
            h.id[2] = 'V';
            h.id[3] = '1';
            h.nameLength = mapName.length();
            h.name = new char[mapName.length() + 1];
            memset(h.name, 0, mapName.length() + 1);
            strncpy(h.name, mapName.toAscii().constData(), mapName.length() + 1);
            h.bonusRect = bRect;
            h.p1 = spawnP1;
            h.p2 = spawnP2;
            h.e1 = spawnE1;
            h.e2 = spawnE2;
            h.e3 = spawnE3;
            h.eagle = eagle;
            stream.writeRawData(h.id, 4);
            writeUInt8(&stream, h.nameLength);
            stream.writeRawData(h.name, h.nameLength);
            writeBonusRect(&stream, h.bonusRect);
            writePoint(&stream, h.p1);
            writePoint(&stream, h.p2);
            writePoint(&stream, h.e1);
            writePoint(&stream, h.e2);
            writePoint(&stream, h.e3);
            writePoint(&stream, h.eagle);
            // ==
            // dane
            for(int x = 0; x < 50; x++)
            {
                for(int y = 0; y < 34; y++)
                {
                    BLOCK_TYPE bt;
                    bt = map[x][y];
                    switch(bt)
                    {
                    case BT_EMPTY:
                        writeUInt8(&stream, TLV1_EMPTY);
                        break;
                    case BT_BRICK:
                        writeUInt8(&stream, TLV1_BRICK);
                        break;
                    case BT_STEEL:
                        writeUInt8(&stream, TLV1_STEEL);
                        break;
                    case BT_SEA:
                        writeUInt8(&stream, TLV1_SEA);
                        break;
                    case BT_JUNGLE:
                        writeUInt8(&stream, TLV1_JUNGLE);
                        break;
                    case BT_ICE:
                        writeUInt8(&stream, TLV1_ICE);
                        break;
                    case BT_EDGE:
                        writeUInt8(&stream, TLV1_EDGE);
                        break;
                    default:
                        writeUInt8(&stream, TLV1_EMPTY);
                        break;
                    }
                }
            }

            file.close();
            changed = false;
        }
    }
}

bool TankMapWidget::load(QString filename)
{
    if(filename.isNull())
    {
        filename = QFileDialog::getOpenFileName(0, tr("Open"), QDir::currentPath(), tr("Tank level files (*.tlv)"));
    }

    if(!filename.isNull())
    {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly))
        {
            QDataStream stream(&file);

            // header
            TLV1_Header h;
            stream.readRawData(h.id, 4);
            if(h.id[0] != 'T' ||
               h.id[1] != 'L' ||
               h.id[2] != 'V' ||
               h.id[3] != '1')
            {
                return false;
            }
            readUInt8(&stream, &h.nameLength);
            h.name = new char[h.nameLength + 1];
            memset(h.name, 0, h.nameLength + 1);
            stream.readRawData(h.name, h.nameLength);
            readBonusRect(&stream, &h.bonusRect);
            readPoint(&stream, &h.p1);
            readPoint(&stream, &h.p2);
            readPoint(&stream, &h.e1);
            readPoint(&stream, &h.e2);
            readPoint(&stream, &h.e3);
            readPoint(&stream, &h.eagle);
            // ==
            // dane
            for(int x = 0; x < 50; x++)
            {
                for(int y = 0; y < 34; y++)
                {
                    TLV1_BLOCK_TYPES bt = TLV1_EMPTY;
                    readUInt8(&stream, (quint8 *) &bt);
                    switch(bt)
                    {
                    case TLV1_EMPTY:
                        map[x][y] = BT_EMPTY;
                        break;
                    case TLV1_BRICK:
                        map[x][y] = BT_BRICK;
                        break;
                    case TLV1_STEEL:
                        map[x][y] = BT_STEEL;
                        break;
                    case TLV1_SEA:
                        map[x][y] = BT_SEA;
                        break;
                    case TLV1_JUNGLE:
                        map[x][y] = BT_JUNGLE;
                        break;
                    case TLV1_ICE:
                        map[x][y] = BT_ICE;
                        break;
                    case TLV1_EDGE:
                        map[x][y] = BT_EDGE;
                        break;
                    default:
                        map[x][y] = BT_EMPTY;
                        break;
                    }
                }
            }
            btnEagle->setEnabled(false);
            btnP1->setEnabled(false);
            btnP2->setEnabled(false);
            btnE->setEnabled(false);
            eagle = h.eagle;
            map[h.eagle.x * 2][h.eagle.y * 2] = BT_EAGLELTOP;
            map[h.eagle.x * 2 + 1][h.eagle.y * 2] = BT_EAGLERTOP;
            map[h.eagle.x * 2][h.eagle.y * 2 + 1] = BT_EAGLELBOT;
            map[h.eagle.x * 2 + 1][h.eagle.y * 2 + 1] = BT_EAGLERBOT;
            spawnP1 = h.p1;
            map[h.p1.x * 2][h.p1.y * 2] = BT_PLAYER1LTOP;
            map[h.p1.x * 2 + 1][h.p1.y * 2] = BT_PLAYER1RTOP;
            map[h.p1.x * 2][h.p1.y * 2 + 1] = BT_PLAYER1LBOT;
            map[h.p1.x * 2 + 1][h.p1.y * 2 + 1] = BT_PLAYER1RBOT;
            spawnP2 = h.p2;
            map[h.p2.x * 2][h.p2.y * 2] = BT_PLAYER2LTOP;
            map[h.p2.x * 2 + 1][h.p2.y * 2] = BT_PLAYER2RTOP;
            map[h.p2.x * 2][h.p2.y * 2 + 1] = BT_PLAYER2LBOT;
            map[h.p2.x * 2 + 1][h.p2.y * 2 + 1] = BT_PLAYER2RBOT;
            spawnE1 = h.e1;
            map[h.e1.x * 2][h.e1.y * 2] = BT_ENEMYLTOP;
            map[h.e1.x * 2 + 1][h.e1.y * 2] = BT_ENEMYRTOP;
            map[h.e1.x * 2][h.e1.y * 2 + 1] = BT_ENEMYLBOT;
            map[h.e1.x * 2 + 1][h.e1.y * 2 + 1] = BT_ENEMYRBOT;
            spawnE2 = h.e2;
            map[h.e2.x * 2][h.e2.y * 2] = BT_ENEMYLTOP;
            map[h.e2.x * 2 + 1][h.e2.y * 2] = BT_ENEMYRTOP;
            map[h.e2.x * 2][h.e2.y * 2 + 1] = BT_ENEMYLBOT;
            map[h.e2.x * 2 + 1][h.e2.y * 2 + 1] = BT_ENEMYRBOT;
            spawnE3 = h.e3;
            map[h.e3.x * 2][h.e3.y * 2] = BT_ENEMYLTOP;
            map[h.e3.x * 2 + 1][h.e3.y * 2] = BT_ENEMYRTOP;
            map[h.e3.x * 2][h.e3.y * 2 + 1] = BT_ENEMYLBOT;
            map[h.e3.x * 2 + 1][h.e3.y * 2 + 1] = BT_ENEMYRBOT;

            mapName = h.name;

            bRect = h.bonusRect;

            file.close();
            changed = false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void TankMapWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBackground(QBrush(QColor(128, 128, 128), Qt::DiagCrossPattern));
    painter.eraseRect(QRect(0, 0, width(), height()));
    painter.fillRect(QRect(0, 0, 800, 544), QColor(0, 0, 0));

    BLOCK_TYPE bt[2][2];
    bt[0][0] = BT_EMPTY;
    bt[1][0] = BT_EMPTY;
    bt[0][1] = BT_EMPTY;
    bt[1][1] = BT_EMPTY;
    switch(selectedTool)
    {
    case EMPTY:
        bt[0][0] = BT_EMPTY;
        bt[1][0] = BT_EMPTY;
        bt[0][1] = BT_EMPTY;
        bt[1][1] = BT_EMPTY;
        break;
    case BRICK:
        bt[0][0] = (parts[1][1] == true ? BT_BRICK : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_BRICK : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_BRICK : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_BRICK : BT_EMPTY);
        break;
    case STEEL:
        bt[0][0] = (parts[1][1] == true ? BT_STEEL : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_STEEL : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_STEEL : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_STEEL : BT_EMPTY);
        break;
    case SEA:
        bt[0][0] = (parts[1][1] == true ? BT_SEA : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_SEA : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_SEA : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_SEA : BT_EMPTY);
        break;
    case JUNGLE:
        bt[0][0] = (parts[1][1] == true ? BT_JUNGLE : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_JUNGLE : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_JUNGLE : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_JUNGLE : BT_EMPTY);
        break;
    case ICE:
        bt[0][0] = (parts[1][1] == true ? BT_ICE : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_ICE : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_ICE : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_ICE : BT_EMPTY);
        break;
    case EAGLE:
        bt[0][0] = BT_EAGLELTOP;
        bt[1][0] = BT_EAGLERTOP;
        bt[0][1] = BT_EAGLELBOT;
        bt[1][1] = BT_EAGLERBOT;
        break;
    case EDGE:
        bt[0][0] = (parts[1][1] == true ? BT_EDGE : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_EDGE : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_EDGE : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_EDGE : BT_EMPTY);
        break;
    case P1:
        bt[0][0] = BT_PLAYER1LTOP;
        bt[1][0] = BT_PLAYER1RTOP;
        bt[0][1] = BT_PLAYER1LBOT;
        bt[1][1] = BT_PLAYER1RBOT;
        break;
    case P2:
        bt[0][0] = BT_PLAYER2LTOP;
        bt[1][0] = BT_PLAYER2RTOP;
        bt[0][1] = BT_PLAYER2LBOT;
        bt[1][1] = BT_PLAYER2RBOT;
        break;
    case E:
        bt[0][0] = BT_ENEMYLTOP;
        bt[1][0] = BT_ENEMYRTOP;
        bt[0][1] = BT_ENEMYLBOT;
        bt[1][1] = BT_ENEMYRBOT;
        break;
    }

    for(int x = 0; x < 50; x++)
    {
        for(int y = 0; y < 34; y++)
        {
            painter.drawPixmap(QPoint(x * 16, y * 16), *blocks, QRect(((quint8) (map[x][y] >> 8)) * 2, ((quint8) map[x][y]) * 2, 16, 16));
        }
    }

    if(curPos.x() != -1 && curPos.y() != -1 && curPos.x() < 800 && curPos.y() < 544)
    {
        painter.drawPixmap(QPoint((curPos.x() / 32) * 32, (curPos.y() / 32) * 32), *blocks, QRect(((quint8) (bt[0][0] >> 8)) * 2, ((quint8) bt[0][0]) * 2, 16, 16));
        painter.drawPixmap(QPoint((curPos.x() / 32) * 32, (curPos.y() / 32) * 32 + 16), *blocks, QRect(((quint8) (bt[0][1] >> 8)) * 2, ((quint8) bt[0][1]) * 2, 16, 16));
        painter.drawPixmap(QPoint((curPos.x() / 32) * 32 + 16, (curPos.y() / 32) * 32), *blocks, QRect(((quint8) (bt[1][0] >> 8)) * 2, ((quint8) bt[1][0]) * 2, 16, 16));
        painter.drawPixmap(QPoint((curPos.x() / 32) * 32 + 16, (curPos.y() / 32) * 32 + 16), *blocks, QRect(((quint8) (bt[1][1] >> 8)) * 2, ((quint8) bt[1][1]) * 2, 16, 16));
    }

    if(drawGrid)
    {
        painter.setPen(QColor(255, 255, 255, 64));
        painter.drawLines(vertLines, 25);
        painter.drawLines(horLines, 17);
    }

    painter.fillRect(bRect.x, bRect.y, bRect.w, bRect.h, QColor(45, 251, 255, 113));
}

void TankMapWidget::mouseMoveEvent(QMouseEvent *event)
{
    curPos = event->pos();
    if(curPos.x() <= -1 || curPos.y() <= -1 || curPos.x() > 800 || curPos.y() > 544)
    {
        return;
    }
    if(event->buttons() == Qt::LeftButton)
    {
        BLOCK_TYPE bt[2][2];
        bt[0][0] = BT_EMPTY;
        bt[1][0] = BT_EMPTY;
        bt[0][1] = BT_EMPTY;
        bt[1][1] = BT_EMPTY;
        switch(selectedTool)
        {
        case EMPTY:
            bt[0][0] = BT_EMPTY;
            bt[1][0] = BT_EMPTY;
            bt[0][1] = BT_EMPTY;
            bt[1][1] = BT_EMPTY;
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case BRICK:
            bt[0][0] = (parts[1][1] == true ? BT_BRICK : BT_EMPTY);
            bt[1][0] = (parts[2][1] == true ? BT_BRICK : BT_EMPTY);
            bt[0][1] = (parts[1][2] == true ? BT_BRICK : BT_EMPTY);
            bt[1][1] = (parts[2][2] == true ? BT_BRICK : BT_EMPTY);
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case STEEL:
            bt[0][0] = (parts[1][1] == true ? BT_STEEL : BT_EMPTY);
            bt[1][0] = (parts[2][1] == true ? BT_STEEL : BT_EMPTY);
            bt[0][1] = (parts[1][2] == true ? BT_STEEL : BT_EMPTY);
            bt[1][1] = (parts[2][2] == true ? BT_STEEL : BT_EMPTY);
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case SEA:
            bt[0][0] = (parts[1][1] == true ? BT_SEA : BT_EMPTY);
            bt[1][0] = (parts[2][1] == true ? BT_SEA : BT_EMPTY);
            bt[0][1] = (parts[1][2] == true ? BT_SEA : BT_EMPTY);
            bt[1][1] = (parts[2][2] == true ? BT_SEA : BT_EMPTY);
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case JUNGLE:
            bt[0][0] = (parts[1][1] == true ? BT_JUNGLE : BT_EMPTY);
            bt[1][0] = (parts[2][1] == true ? BT_JUNGLE : BT_EMPTY);
            bt[0][1] = (parts[1][2] == true ? BT_JUNGLE : BT_EMPTY);
            bt[1][1] = (parts[2][2] == true ? BT_JUNGLE : BT_EMPTY);
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case ICE:
            bt[0][0] = (parts[1][1] == true ? BT_ICE : BT_EMPTY);
            bt[1][0] = (parts[2][1] == true ? BT_ICE : BT_EMPTY);
            bt[0][1] = (parts[1][2] == true ? BT_ICE : BT_EMPTY);
            bt[1][1] = (parts[2][2] == true ? BT_ICE : BT_EMPTY);
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case EAGLE:
            return;
        case EDGE:
            bt[0][0] = (parts[1][1] == true ? BT_EDGE : BT_EMPTY);
            bt[1][0] = (parts[2][1] == true ? BT_EDGE : BT_EMPTY);
            bt[0][1] = (parts[1][2] == true ? BT_EDGE : BT_EMPTY);
            bt[1][1] = (parts[2][2] == true ? BT_EDGE : BT_EMPTY);
            if(curPos.x() / 32 == eagle.x && curPos.y() / 32 == eagle.y)
            {
                eagle.x = eagle.y = -1;
                btnEagle->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP1.x && curPos.y() / 32 == spawnP1.y)
            {
                spawnP1.x = spawnP1.y = -1;
                btnP1->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnP2.x && curPos.y() / 32 == spawnP2.y)
            {
                spawnP2.x = spawnP2.y = -1;
                btnP2->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE1.x && curPos.y() / 32 == spawnE1.y)
            {
                spawnE1.x = spawnE1.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE2.x && curPos.y() / 32 == spawnE2.y)
            {
                spawnE2.x = spawnE2.y = -1;
                btnE->setEnabled(true);
            }
            if(curPos.x() / 32 == spawnE3.x && curPos.y() / 32 == spawnE3.y)
            {
                spawnE3.x = spawnE3.y = -1;
                btnE->setEnabled(true);
            }
            break;
        case P1:
            return;
        case P2:
            return;
        case E:
            return;
        }
        int x = (curPos.x() / 32) * 2;
        int y = (curPos.y() / 32) * 2;
        map[x][y] = bt[0][0];
        map[x + 1][y] = bt[1][0];
        map[x][y + 1] = bt[0][1];
        map[x + 1][y + 1] = bt[1][1];
        changed = true;
    }
    repaint();
}

void TankMapWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if(pos.x() <= -1 || pos.y() <= -1 || pos.x() > 800 || pos.y() > 544)
    {
        return;
    }

    BLOCK_TYPE bt[2][2];
    bt[0][0] = BT_EMPTY;
    bt[1][0] = BT_EMPTY;
    bt[0][1] = BT_EMPTY;
    bt[1][1] = BT_EMPTY;
    switch(selectedTool)
    {
    case EMPTY:
        bt[0][0] = BT_EMPTY;
        bt[1][0] = BT_EMPTY;
        bt[0][1] = BT_EMPTY;
        bt[1][1] = BT_EMPTY;
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case BRICK:
        bt[0][0] = (parts[1][1] == true ? BT_BRICK : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_BRICK : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_BRICK : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_BRICK : BT_EMPTY);
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case STEEL:
        bt[0][0] = (parts[1][1] == true ? BT_STEEL : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_STEEL : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_STEEL : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_STEEL : BT_EMPTY);
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case SEA:
        bt[0][0] = (parts[1][1] == true ? BT_SEA : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_SEA : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_SEA : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_SEA : BT_EMPTY);
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case JUNGLE:
        bt[0][0] = (parts[1][1] == true ? BT_JUNGLE : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_JUNGLE : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_JUNGLE : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_JUNGLE : BT_EMPTY);
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case ICE:
        bt[0][0] = (parts[1][1] == true ? BT_ICE : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_ICE : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_ICE : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_ICE : BT_EMPTY);
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case EAGLE:
        bt[0][0] = BT_EAGLELTOP;
        bt[1][0] = BT_EAGLERTOP;
        bt[0][1] = BT_EAGLELBOT;
        bt[1][1] = BT_EAGLERBOT;
        eagle.x = pos.x() / 32;
        eagle.y = pos.y() / 32;
        btnEagle->setEnabled(false);
        btnEmpty->click();
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case EDGE:
        bt[0][0] = (parts[1][1] == true ? BT_EDGE : BT_EMPTY);
        bt[1][0] = (parts[2][1] == true ? BT_EDGE : BT_EMPTY);
        bt[0][1] = (parts[1][2] == true ? BT_EDGE : BT_EMPTY);
        bt[1][1] = (parts[2][2] == true ? BT_EDGE : BT_EMPTY);
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case P1:
        bt[0][0] = BT_PLAYER1LTOP;
        bt[1][0] = BT_PLAYER1RTOP;
        bt[0][1] = BT_PLAYER1LBOT;
        bt[1][1] = BT_PLAYER1RBOT;
        spawnP1.x = pos.x() / 32;
        spawnP1.y = pos.y() / 32;
        btnP1->setEnabled(false);
        btnEmpty->click();
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case P2:
        bt[0][0] = BT_PLAYER2LTOP;
        bt[1][0] = BT_PLAYER2RTOP;
        bt[0][1] = BT_PLAYER2LBOT;
        bt[1][1] = BT_PLAYER2RBOT;
        spawnP2.x = pos.x() / 32;
        spawnP2.y = pos.y() / 32;
        btnP2->setEnabled(false);
        btnEmpty->click();
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE1.x && pos.y() / 32 == spawnE1.y)
        {
            spawnE1.x = spawnE1.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE2.x && pos.y() / 32 == spawnE2.y)
        {
            spawnE2.x = spawnE2.y = -1;
            btnE->setEnabled(true);
        }
        if(pos.x() / 32 == spawnE3.x && pos.y() / 32 == spawnE3.y)
        {
            spawnE3.x = spawnE3.y = -1;
            btnE->setEnabled(true);
        }
        break;
    case E:
        bt[0][0] = BT_ENEMYLTOP;
        bt[1][0] = BT_ENEMYRTOP;
        bt[0][1] = BT_ENEMYLBOT;
        bt[1][1] = BT_ENEMYRBOT;
        if(spawnE1.x != -1 && spawnE1.y != -1)
        {
            if(spawnE2.x != -1 && spawnE2.y != -1)
            {
                if(spawnE3.x != -1 && spawnE3.y != -1)
                {
                    return;
                }
                else
                {
                    spawnE3.x = pos.x() / 32;
                    spawnE3.y = pos.y() / 32;
                    if(spawnE1.x != -1 && spawnE1.y != -1 && spawnE2.x != -1 && spawnE2.y != -1)
                    {
                        btnE->setEnabled(false);
                        btnEmpty->click();
                    }
                }
            }
            else
            {
                spawnE2.x = pos.x() / 32;
                spawnE2.y = pos.y() / 32;
                if(spawnE1.x != -1 && spawnE1.y != -1 && spawnE3.x != -1 && spawnE3.y != -1)
                {
                    btnE->setEnabled(false);
                    btnEmpty->click();
                }
            }
        }
        else
        {
            spawnE1.x = pos.x() / 32;
            spawnE1.y = pos.y() / 32;
            if(spawnE2.x != -1 && spawnE2.y != -1 && spawnE3.x != -1 && spawnE3.y != -1)
            {
                btnE->setEnabled(false);
                btnEmpty->click();
            }
        }
        if(pos.x() / 32 == eagle.x && pos.y() / 32 == eagle.y)
        {
            eagle.x = eagle.y = -1;
            btnEagle->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP1.x && pos.y() / 32 == spawnP1.y)
        {
            spawnP1.x = spawnP1.y = -1;
            btnP1->setEnabled(true);
        }
        if(pos.x() / 32 == spawnP2.x && pos.y() / 32 == spawnP2.y)
        {
            spawnP2.x = spawnP2.y = -1;
            btnP2->setEnabled(true);
        }
        break;
    }
    int x = (pos.x() / 32) * 2;
    int y = (pos.y() / 32) * 2;
    map[x][y] = bt[0][0];
    map[x + 1][y] = bt[1][0];
    map[x][y + 1] = bt[0][1];
    map[x + 1][y + 1] = bt[1][1];
    changed = true;
    repaint();
}

void TankMapWidget::writeUInt8(QDataStream *stream, quint8 i)
{
    char buffer[1];
    buffer[0] = i;
    stream->writeRawData(buffer, 1);
}

void TankMapWidget::writeUInt16(QDataStream *stream, quint16 i)
{
    char buffer[2];
    // little endian
    buffer[0] = (quint8) i;
    buffer[1] = (quint8) (i >> 8);
    stream->writeRawData(buffer, 2);
}

void TankMapWidget::writeBonusRect(QDataStream *stream, TLV1_BonusRect br)
{
    writeUInt16(stream, br.x);
    writeUInt16(stream, br.y);
    writeUInt16(stream, br.w);
    writeUInt16(stream, br.h);
}

void TankMapWidget::writePoint(QDataStream *stream, TLV1_Point p)
{
    writeUInt8(stream, p.x);
    writeUInt8(stream, p.y);
}

void TankMapWidget::readUInt8(QDataStream *stream, quint8 *i)
{
    char buffer[1];
    stream->readRawData(buffer, 1);
    *i = (quint8) buffer[0];
}

void TankMapWidget::readUInt16(QDataStream *stream, quint16 *i)
{
    char buffer[2];
    quint8 ubuffer[2];
    // little endian
    stream->readRawData(&buffer[0], 1);
    stream->readRawData(&buffer[1], 1);
    ubuffer[0] = buffer[0];
    ubuffer[1] = buffer[1];
    *i = (quint16) ((ubuffer[1] << 8) + ubuffer[0]);
}

void TankMapWidget::readBonusRect(QDataStream *stream, TLV1_BonusRect *br)
{
    readUInt16(stream, &br->x);
    readUInt16(stream, &br->y);
    readUInt16(stream, &br->w);
    readUInt16(stream, &br->h);
}

void TankMapWidget::readPoint(QDataStream *stream, TLV1_Point *p)
{
    readUInt8(stream, (quint8 *) &p->x);
    readUInt8(stream, (quint8 *) &p->y);
}
