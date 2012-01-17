/*
    Copyright 2011, 2012 Kacper Kasper <kacperkasper@gmail.com>

    This file is part of Tank.

    Tank is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tank is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tank.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <cstring>
#include "Config.h"

using namespace std;

Config::Config()
{
    // Default values
    m_bFullscreen = false;
    m_iVideoDriver = 1; // OpenGL
    m_kP1Controller = KEYBOARD;
    m_kP1Controls.iUp = 273; // up arrow
    m_kP1Controls.iDown = 274; // down arrow
    m_kP1Controls.iLeft = 276; // left arrow
    m_kP1Controls.iRight = 275; // right arrow
    m_kP1Controls.iShoot = 305; // right ctrl
    m_kP2Controller = KEYBOARD;
    m_kP2Controls.iUp = 119; // w
    m_kP2Controls.iDown = 115; // s
    m_kP2Controls.iLeft = 97; // a
    m_kP2Controls.iRight = 100; // d
    m_kP2Controls.iShoot = 113; // q

    ConfigItem akItems[] = {
        { "fullscreen",         &m_bFullscreen,         CIT_BOOL },
        { "videodriver",        &m_iVideoDriver,        CIT_INT },
        { "p1controller",       &m_kP1Controller,       CIT_INT },
        { "p1up",               &m_kP1Controls.iUp,     CIT_INT },
        { "p1down",             &m_kP1Controls.iDown,   CIT_INT },
        { "p1left",             &m_kP1Controls.iLeft,   CIT_INT },
        { "p1right",            &m_kP1Controls.iRight,  CIT_INT },
        { "p1shoot",            &m_kP1Controls.iShoot,  CIT_INT },
        { "p2controller",       &m_kP2Controller,       CIT_INT },
        { "p2up",               &m_kP2Controls.iUp,     CIT_INT },
        { "p2down",             &m_kP2Controls.iDown,   CIT_INT },
        { "p2left",             &m_kP2Controls.iLeft,   CIT_INT },
        { "p2right",            &m_kP2Controls.iRight,  CIT_INT },
        { "p2shoot",            &m_kP2Controls.iShoot,  CIT_INT },
    };
    m_iItemCount = sizeof(akItems) / sizeof(akItems[0]);
    m_akItems = new ConfigItem[m_iItemCount];
    memcpy(m_akItems, &akItems, sizeof(akItems));
}

Config::~Config()
{
    if(m_akItems != NULL)
    {
        delete [] m_akItems;
        m_akItems = NULL;
    }
}

void Config::ReadFromFile(const char *strFilename)
{
    ifstream kFile;

    kFile.open(strFilename);
    if(kFile.is_open())
    {
        while(!kFile.eof())
        {
            char line[128];

            kFile.getline(line, 128);

            // Line parsing
            char identifier[64] = "";
            char value[64] = "";
            int pos;
            int len;

            len = strlen(line);
            pos = (long) strchr(line, '=');
            if(pos != NULL)
            {
                pos = pos - (long) line + 1;
                strncpy(identifier, line, pos - 1);
                identifier[pos - 1] = 0;
                strncpy(value, &line[pos], len - pos);
                value[len - pos] = 0;
            }

            for(int i = 0; i < m_iItemCount; i++)
            {
                if(strcmp(identifier, m_akItems[i].strIdentifier) == 0)
                {
                    switch(m_akItems[i].kType)
                    {
                    case CIT_INT:
                        *((int *) m_akItems[i].pValue) = atoi(value);
                        break;
                    case CIT_BOOL:
                        *((bool *) m_akItems[i].pValue) = (bool) atoi(value);
                        break;
                    case CIT_STRING:
                        *((char **) m_akItems[i].pValue) = value;
                        break;
                    }
                }
            }
        }
        kFile.close();
    }
}

void Config::SaveToFile(const char *strFilename)
{
    ofstream kFile;

    kFile.open(strFilename);

    if(kFile.is_open())
    {
        char line[128];
        
        kFile << "#Generated by Tank. DO NOT modify." << endl;
        for(int i = 0; i < m_iItemCount; i++)
        {
            switch(m_akItems[i].kType)
            {
            case CIT_INT:
                sprintf(line, "%s=%d", m_akItems[i].strIdentifier, *((int *) m_akItems[i].pValue));
                break;
            case CIT_BOOL:
                sprintf(line, "%s=%d", m_akItems[i].strIdentifier, *((bool *) m_akItems[i].pValue));
                break;
            case CIT_STRING:
                sprintf(line, "%s=%s", m_akItems[i].strIdentifier, *((char **) m_akItems[i].pValue));
                break;
            }
            kFile << line << endl;
        }
        kFile.close();
    }
}