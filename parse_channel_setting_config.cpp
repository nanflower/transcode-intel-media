#include "parse_channel_setting_config.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>
#include <unistd.h>
const QString CHANNEL_SETTING_CONFIG_FILE( "config/channel_setting_config.xml" );

// 配置文件节点字符串
const QString ROOT_NODE( "configs" );
const QString CHANNEL_NODE( "channel" );
const QString NUM_NODE( "num" );
const QString QUALITY_NODE( "quality" );
const QString RTMP_ENABLE_NODE1( "rtmpEnable1" );
const QString RTMP_ADDR_NODE1( "rtmpAddr1" );
const QString RTMP_AUDIO_DELAY_NODE1( "rtmpAudioDelay1" );
const QString RTMP_ENABLE_NODE2( "rtmpEnable2" );
const QString RTMP_ADDR_NODE2( "rtmpAddr2" );
const QString RTMP_AUDIO_DELAY_NODE2( "rtmpAudioDelay2" );
const QString UDP_RTP_ENABLE_NODE( "udpRtpEnable" );
const QString UDP_RTP_ADDR_NODE( "udpRtpAddr" );
const QString UDP_RTP_IP_NODE( "udpRtpIp" );
const QString UDP_RTP_PORT_NODE( "udpRtpPort" );
const QString UDP_RTP_AUDIO_DELAY_NODE( "udpRtpAudioDelay" );
const QString INPUT_VIDEO_FORMAT_NODE("InputVideoFormat");
const QString OUTPUT_VIDEO_FORMAT_NODE("OutputVideoFormat");

CParseChannelSettingConfig* CParseChannelSettingConfig::m_spParseXmlConfig = NULL;
CParseChannelSettingConfig::CParseChannelSettingConfig()
{
}

CParseChannelSettingConfig::~CParseChannelSettingConfig()
{
}

CParseChannelSettingConfig* CParseChannelSettingConfig::NewInstance( )
{
    if( NULL == m_spParseXmlConfig )
    {
        m_spParseXmlConfig = new CParseChannelSettingConfig;
        if( m_spParseXmlConfig )
            m_spParseXmlConfig->ReadChannelSettingConfigFile();
    }

    if( m_spParseXmlConfig )
        m_spParseXmlConfig->Addref();

    return m_spParseXmlConfig;
}

bool CParseChannelSettingConfig::DestoryInstance()
{
    if( NULL == m_spParseXmlConfig )
        return true;

    if( 0 == m_spParseXmlConfig->Release() )
    {
        delete m_spParseXmlConfig;
        m_spParseXmlConfig = NULL;
    }

    return true;
}

bool CParseChannelSettingConfig::GetChannelSettingInfo( unsigned int nChannelNum,
                                                        CHANNEL_SETTING_INFO* pChannelSettingInfo, bool bNew )
{
    if( !IsValidIndex(nChannelNum) )
        return false;

    if( NULL == pChannelSettingInfo )
        return false;

    for( int i=0; i< m_vecChannelSettingInfo.count(); i++ )
    {
        if( nChannelNum == m_vecChannelSettingInfo[i].nChannelNum )
        {
            pChannelSettingInfo->nChannelNum = bNew?m_vecNewChannelSettingInfo[i].nChannelNum:m_vecChannelSettingInfo[i].nChannelNum;
            pChannelSettingInfo->enuQuality = bNew?m_vecNewChannelSettingInfo[i].enuQuality:m_vecChannelSettingInfo[i].enuQuality;
            pChannelSettingInfo->bRtmpEnable1 = bNew?m_vecNewChannelSettingInfo[i].bRtmpEnable1:m_vecChannelSettingInfo[i].bRtmpEnable1;
            pChannelSettingInfo->strRtmpAddr1 = bNew?m_vecNewChannelSettingInfo[i].strRtmpAddr1:m_vecChannelSettingInfo[i].strRtmpAddr1;
            pChannelSettingInfo->nRtmpAudioDelay1 = bNew?m_vecNewChannelSettingInfo[i].nRtmpAudioDelay1:m_vecChannelSettingInfo[i].nRtmpAudioDelay1;
            pChannelSettingInfo->bRtmpEnable2 = bNew?m_vecNewChannelSettingInfo[i].bRtmpEnable2:m_vecChannelSettingInfo[i].bRtmpEnable2;
            pChannelSettingInfo->strRtmpAddr2 = bNew?m_vecNewChannelSettingInfo[i].strRtmpAddr2:m_vecChannelSettingInfo[i].strRtmpAddr2;
            pChannelSettingInfo->nRtmpAudioDelay2 = bNew?m_vecNewChannelSettingInfo[i].nRtmpAudioDelay2:m_vecChannelSettingInfo[i].nRtmpAudioDelay2;
            pChannelSettingInfo->bUdpOrRtpEnable = bNew?m_vecNewChannelSettingInfo[i].bUdpOrRtpEnable:m_vecChannelSettingInfo[i].bUdpOrRtpEnable;
            pChannelSettingInfo->bUdpOrRtpAddr = bNew?m_vecNewChannelSettingInfo[i].bUdpOrRtpAddr:m_vecChannelSettingInfo[i].bUdpOrRtpAddr;
            pChannelSettingInfo->strUdpOrRtpIp = bNew?m_vecNewChannelSettingInfo[i].strUdpOrRtpIp:m_vecChannelSettingInfo[i].strUdpOrRtpIp;
            pChannelSettingInfo->nUdpOrRtpPort = bNew?m_vecNewChannelSettingInfo[i].nUdpOrRtpPort:m_vecChannelSettingInfo[i].nUdpOrRtpPort;
            pChannelSettingInfo->nUdpOrRtpAudioDelay = bNew?m_vecNewChannelSettingInfo[i].nUdpOrRtpAudioDelay:m_vecChannelSettingInfo[i].nUdpOrRtpAudioDelay;
            pChannelSettingInfo->nInputVideoFormat = bNew?m_vecNewChannelSettingInfo[i].nInputVideoFormat:m_vecChannelSettingInfo[i].nInputVideoFormat;
            pChannelSettingInfo->nOutputVideoFormat = bNew?m_vecNewChannelSettingInfo[i].nOutputVideoFormat:m_vecChannelSettingInfo[i].nOutputVideoFormat;
            break;
        }
    }
    return true;
}

QUALITY_ID CParseChannelSettingConfig::GetChannelQuality( unsigned int nChannelNum )
{
    QUALITY_ID ret = EXCELLENT;
    if( !IsValidIndex(nChannelNum) )
        return ret;

    for( int i=0; i< m_vecChannelSettingInfo.count(); i++ )
    {
        if( nChannelNum == m_vecChannelSettingInfo[i].nChannelNum )
        {
            ret = m_vecChannelSettingInfo[i].enuQuality;
            break;
        }
    }
    return ret;
}

QString CParseChannelSettingConfig::GetChannelRtmpAddr( unsigned int nChannelNum, bool bRtmp1 )
{
    if( !IsValidIndex(nChannelNum) )
        return NULL;

    for( int i=0; i< m_vecChannelSettingInfo.count(); i++ )
    {
        if( nChannelNum == m_vecChannelSettingInfo[i].nChannelNum )
        {
            if( bRtmp1 )
                return m_vecChannelSettingInfo[i].strRtmpAddr1;
            else
                return m_vecChannelSettingInfo[i].strRtmpAddr2;
        }
    }
    return NULL;
}

bool CParseChannelSettingConfig::GetChannelRtmpEnable( unsigned int nChannelNum, bool bRtmp1 )
{
    if( !IsValidIndex(nChannelNum) )
        return false;

    for( int i=0; i< m_vecChannelSettingInfo.count(); i++ )
    {
        if( nChannelNum == m_vecChannelSettingInfo[i].nChannelNum )
        {
            if( bRtmp1 )
                return m_vecChannelSettingInfo[i].bRtmpEnable1;
            else
                return m_vecChannelSettingInfo[i].bRtmpEnable2;
        }
    }
    return false;
}

bool CParseChannelSettingConfig::GetChannelUdpOrRtpEnable( unsigned int nChannelNum )
{
    if( !IsValidIndex(nChannelNum) )
        return false;

    for( int i=0; i< m_vecChannelSettingInfo.count(); i++ )
    {
        if( nChannelNum == m_vecChannelSettingInfo[i].nChannelNum )
        {
            return m_vecChannelSettingInfo[i].bUdpOrRtpEnable;
        }
    }
    return false;
}

unsigned int CParseChannelSettingConfig::GetChannelRtmpDelay( unsigned int nChannelNum, bool bRtmp1 )
{
    if( !IsValidIndex(nChannelNum) )
        return 0;

    for( int i=0; i< m_vecChannelSettingInfo.count(); i++ )
    {
        if( nChannelNum == m_vecChannelSettingInfo[i].nChannelNum )
        {
            return bRtmp1? m_vecChannelSettingInfo[i].nRtmpAudioDelay1:m_vecChannelSettingInfo[i].nRtmpAudioDelay2;
        }
    }
    return 0;
}

void CParseChannelSettingConfig::SaveChannelSettingInfo( CHANNEL_SETTING_INFO ChannelSettingInfo )
{
    bool bSaveToFile = false;
    for( int i=0; i< m_vecNewChannelSettingInfo.count(); i++ )
    {
        PCHANNEL_SETTING_INFO pCurInfo = &m_vecNewChannelSettingInfo[i];
        if( pCurInfo->nChannelNum == ChannelSettingInfo.nChannelNum )
        {
            if( pCurInfo->enuQuality != ChannelSettingInfo.enuQuality )
            {
                bSaveToFile = true;
                pCurInfo->enuQuality = ChannelSettingInfo.enuQuality;
            }
            if( pCurInfo->bRtmpEnable1 != ChannelSettingInfo.bRtmpEnable1 )
            {
                bSaveToFile = true;
                pCurInfo->bRtmpEnable1 = ChannelSettingInfo.bRtmpEnable1;
            }
            if( pCurInfo->strRtmpAddr1 != ChannelSettingInfo.strRtmpAddr1 )
            {
                bSaveToFile = true;
                pCurInfo->strRtmpAddr1 = ChannelSettingInfo.strRtmpAddr1;
            }
            if( pCurInfo->nRtmpAudioDelay1 != ChannelSettingInfo.nRtmpAudioDelay1 )
            {
                bSaveToFile = true;
                pCurInfo->nRtmpAudioDelay1 = ChannelSettingInfo.nRtmpAudioDelay1;
            }
            if( pCurInfo->bRtmpEnable2 != ChannelSettingInfo.bRtmpEnable2 )
            {
                bSaveToFile = true;
                pCurInfo->bRtmpEnable2 = ChannelSettingInfo.bRtmpEnable2;
            }
            if( pCurInfo->strRtmpAddr2 != ChannelSettingInfo.strRtmpAddr2 )
            {
                bSaveToFile = true;
                pCurInfo->strRtmpAddr2 = ChannelSettingInfo.strRtmpAddr2;
            }
            if( pCurInfo->nRtmpAudioDelay2 != ChannelSettingInfo.nRtmpAudioDelay2 )
            {
                bSaveToFile = true;
                pCurInfo->nRtmpAudioDelay2 = ChannelSettingInfo.nRtmpAudioDelay2;
            }
            if( pCurInfo->bUdpOrRtpEnable != ChannelSettingInfo.bUdpOrRtpEnable )
            {
                bSaveToFile = true;
                pCurInfo->bUdpOrRtpEnable = ChannelSettingInfo.bUdpOrRtpEnable;
            }
            if( pCurInfo->bUdpOrRtpAddr != ChannelSettingInfo.bUdpOrRtpAddr )
            {
                bSaveToFile = true;
                pCurInfo->bUdpOrRtpAddr = ChannelSettingInfo.bUdpOrRtpAddr;
            }
            if( pCurInfo->strUdpOrRtpIp != ChannelSettingInfo.strUdpOrRtpIp )
            {
                bSaveToFile = true;
                pCurInfo->strUdpOrRtpIp = ChannelSettingInfo.strUdpOrRtpIp;
            }
            if( pCurInfo->nUdpOrRtpPort != ChannelSettingInfo.nUdpOrRtpPort )
            {
                bSaveToFile = true;
                pCurInfo->nUdpOrRtpPort = ChannelSettingInfo.nUdpOrRtpPort;
            }
            if( pCurInfo->nUdpOrRtpAudioDelay != ChannelSettingInfo.nUdpOrRtpAudioDelay )
            {
                bSaveToFile = true;
                pCurInfo->nUdpOrRtpAudioDelay = ChannelSettingInfo.nUdpOrRtpAudioDelay;
            }
            if( pCurInfo->nInputVideoFormat != ChannelSettingInfo.nInputVideoFormat )
            {
                bSaveToFile = true;
                pCurInfo->nInputVideoFormat = ChannelSettingInfo.nInputVideoFormat;
            }
            if( pCurInfo->nOutputVideoFormat != ChannelSettingInfo.nOutputVideoFormat )
            {
                bSaveToFile = true;
                pCurInfo->nOutputVideoFormat = ChannelSettingInfo.nOutputVideoFormat;
            }
            break;
        }
    }
    if( bSaveToFile )
        SaveFile();
}

bool CParseChannelSettingConfig::IsChangedVideoFormat()
{
    if( m_vecChannelSettingInfo.count() > 0 && m_vecChannelSettingInfo.count() != m_vecNewChannelSettingInfo.count() )
        return false;
    for( int i= 0; i<m_vecChannelSettingInfo.count(); i++ )
    {
        if( m_vecChannelSettingInfo[i].nInputVideoFormat != m_vecNewChannelSettingInfo[i].nInputVideoFormat ||
                m_vecChannelSettingInfo[i].nOutputVideoFormat != m_vecNewChannelSettingInfo[i].nOutputVideoFormat )
            return true;
    }
    return false;
}

void CParseChannelSettingConfig::SaveChannelVideoFormat( QVector<INPUT_VIDEO_FORMAT> vecVideoFormat )
{
    if( m_vecNewChannelSettingInfo.count() != vecVideoFormat.count() )
        return;

    bool bChanged = false;
    for( int i=0; i<m_vecNewChannelSettingInfo.count(); i++ )
    {
        if( m_vecNewChannelSettingInfo[i].nInputVideoFormat != (unsigned int)vecVideoFormat[i] && \
                (vecVideoFormat[i] == INPUT_1080I_50_60 || vecVideoFormat[i] == INPUT_720P_50_60) )
        {
            bChanged = true;
            m_vecNewChannelSettingInfo[i].nInputVideoFormat = vecVideoFormat[i];
            m_vecNewChannelSettingInfo[i].nOutputVideoFormat = (vecVideoFormat[i] == INPUT_1080I_50_60)?OUTPUT_1920_1080I_25_30:OUTPUT_720P_50_60;
        }
    }

    if( bChanged )
        SaveFile();
}

// private
int CParseChannelSettingConfig::Addref()
{
    return ++m_nReference;
}

int CParseChannelSettingConfig::Release()
{
    return --m_nReference;
}

bool CParseChannelSettingConfig::ReadFile( QDomDocument& doc, const QString& fileName )
{
    QFile file( fileName );
    // 检查文件是否存在
    if( !file.exists() )
    {
        qDebug() << fileName << "is not exist!";
        return false;
    }

    // 打开文件
    if( !file.open(QFile::ReadOnly | QFile::Text) )
    {
        qWarning() << "Error: Cannot read file " << qPrintable( fileName )
                   << ": " << qPrintable( file.errorString() );
        return false;
    }

    // 指定内容给QDomDocument解析，并检查配置文件的格式
    QString strErrorInfo;
    int nErrorLine, nErrorCol;
    if( !doc.setContent( &file, true, &strErrorInfo, &nErrorLine, &nErrorCol ) )
    {
        file.close();
        qWarning() << "Error:" << strErrorInfo << " line:" << nErrorLine << " col:" << nErrorCol;
        return false;
    }
    file.close();

    return true;
}

bool CParseChannelSettingConfig::IsExist( CHANNEL_SETTING_INFO NewChannelSettingInfo )
{
    int nCount = m_vecChannelSettingInfo.count();
    for( int i=0; i<nCount; ++i )
    {
        if( m_vecChannelSettingInfo[i].nChannelNum == NewChannelSettingInfo.nChannelNum )
            return true;
    }

    return false;
}

bool CParseChannelSettingConfig::SaveFile()
{
    QString tempFile = "channel_temp.xml";
    if( QFile::exists( tempFile ) )
    {
        if( !QFile::remove( tempFile ) )
            return false;
    }
    QFile writeFile( tempFile );
    if( !writeFile.open(QFile::ReadWrite| QFile::Text) )
        return false;

    QXmlStreamWriter   XmlWrite;
    XmlWrite.setDevice( &writeFile );
    XmlWrite.setAutoFormatting( true );
    XmlWrite.writeStartDocument();
    XmlWrite.writeStartElement( ROOT_NODE );
    int nCount = m_vecNewChannelSettingInfo.count();
    for( int i=0; i<nCount; ++i )
    {
        CHANNEL_SETTING_INFO channelSettingInfo =m_vecNewChannelSettingInfo[i];
        XmlWrite.writeStartElement( CHANNEL_NODE );

        XmlWrite.writeTextElement( NUM_NODE, QString::number(channelSettingInfo.nChannelNum) );
        XmlWrite.writeTextElement( QUALITY_NODE, QString::number(channelSettingInfo.enuQuality) );
        XmlWrite.writeTextElement( RTMP_ENABLE_NODE1, QString::number(channelSettingInfo.bRtmpEnable1? 1:0) );
        XmlWrite.writeTextElement( RTMP_ADDR_NODE1, channelSettingInfo.strRtmpAddr1 );
        XmlWrite.writeTextElement( RTMP_AUDIO_DELAY_NODE1, QString::number(channelSettingInfo.nRtmpAudioDelay1) );

        XmlWrite.writeTextElement( RTMP_ENABLE_NODE2, QString::number(channelSettingInfo.bRtmpEnable2? 1:0) );
        XmlWrite.writeTextElement( RTMP_ADDR_NODE2, channelSettingInfo.strRtmpAddr2 );
        XmlWrite.writeTextElement( RTMP_AUDIO_DELAY_NODE2, QString::number(channelSettingInfo.nRtmpAudioDelay2) );

        XmlWrite.writeTextElement( UDP_RTP_ENABLE_NODE, QString::number(channelSettingInfo.bUdpOrRtpEnable? 1:0) );
        XmlWrite.writeTextElement( UDP_RTP_ADDR_NODE, QString::number(channelSettingInfo.bUdpOrRtpAddr?1:0) );
        XmlWrite.writeTextElement( UDP_RTP_IP_NODE, channelSettingInfo.strUdpOrRtpIp );
        XmlWrite.writeTextElement( UDP_RTP_PORT_NODE, QString::number(channelSettingInfo.nUdpOrRtpPort) );
        XmlWrite.writeTextElement( UDP_RTP_AUDIO_DELAY_NODE, QString::number(channelSettingInfo.nUdpOrRtpAudioDelay) );
        XmlWrite.writeTextElement( INPUT_VIDEO_FORMAT_NODE, QString::number(channelSettingInfo.nInputVideoFormat));
        XmlWrite.writeTextElement( OUTPUT_VIDEO_FORMAT_NODE, QString::number(channelSettingInfo.nOutputVideoFormat));
        XmlWrite.writeEndElement();
    }

    XmlWrite.writeEndDocument();
    writeFile.close();

    // 临时文件写完后，将临时文件copy到备份文件
    QString strSaveFile = CHANNEL_SETTING_CONFIG_FILE;
    QFile::remove( strSaveFile );
    bool bRet = QFile::rename( tempFile, strSaveFile );
    sync();
    return bRet;
}


bool CParseChannelSettingConfig::ReadChannelSettingConfigFile()
{
    // read file
    QDomDocument doc;
    QString strFileName = CHANNEL_SETTING_CONFIG_FILE;
    if( !ReadFile( doc, strFileName ) )
        return false;

    // root 节点 (configs)
    QDomElement rootElem = doc.documentElement();
    if( ROOT_NODE != rootElem.nodeName() )
    {
        qWarning() << qPrintable( strFileName ) << "format is error, Please checkout your xml file!";
        return false;
    }

    // channel 节点
    QDomNodeList channelList = rootElem.childNodes(); //获得channel列表
    int nCount = channelList.count();
    for( int i = 0; i < nCount; ++i )
    {
        QDomElement channelElem = channelList.at(i).toElement();
        if( !channelElem.isElement() \
                || 0 != QString::compare( channelElem.nodeName(), CHANNEL_NODE, Qt::CaseInsensitive )  )
        {
            continue;
        }

        QDomNodeList channelChildList = channelElem.childNodes(); //获得channel子列表
        CHANNEL_SETTING_INFO channelSettingInfo;
        int nChildCount = channelChildList.count();
        for( int j = 0; j < nChildCount; ++j )
        {
            QDomElement channelSettingChildElem = channelChildList.at(j).toElement();
            if( !channelSettingChildElem.isElement() )
                continue;

            QString strNodeName = channelSettingChildElem.nodeName();
            QString strText = channelSettingChildElem.text();
            int nValue = strText.toInt();
            if( 0 == QString::compare( strNodeName, NUM_NODE, Qt::CaseInsensitive )  )
                channelSettingInfo.nChannelNum = nValue;
            else if( 0 == QString::compare( strNodeName, QUALITY_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.enuQuality =(QUALITY_ID)nValue;
            else if( 0 == QString::compare( strNodeName, RTMP_ENABLE_NODE1, Qt::CaseInsensitive ) )
                channelSettingInfo.bRtmpEnable1 = ( 1 == nValue );
            else if( 0 == QString::compare( strNodeName, RTMP_ADDR_NODE1, Qt::CaseInsensitive ) )
                channelSettingInfo.strRtmpAddr1 = strText;
            else if( 0 == QString::compare( strNodeName, RTMP_AUDIO_DELAY_NODE1, Qt::CaseInsensitive ) )
                channelSettingInfo. nRtmpAudioDelay1= nValue;

            else if( 0 == QString::compare( strNodeName, RTMP_ENABLE_NODE2, Qt::CaseInsensitive ) )
                channelSettingInfo.bRtmpEnable2 = ( 1 == nValue );
            else if( 0 == QString::compare( strNodeName, RTMP_ADDR_NODE2, Qt::CaseInsensitive ) )
                channelSettingInfo.strRtmpAddr2 = strText;
            else if( 0 == QString::compare( strNodeName, RTMP_AUDIO_DELAY_NODE2, Qt::CaseInsensitive ) )
                channelSettingInfo. nRtmpAudioDelay2= nValue;

            else if( 0 == QString::compare( strNodeName, UDP_RTP_ENABLE_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.bUdpOrRtpEnable = ( 1 == nValue );
            else if( 0 == QString::compare( strNodeName, UDP_RTP_ADDR_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.bUdpOrRtpAddr =  ( 1 == nValue );
            else if( 0 == QString::compare( strNodeName, UDP_RTP_IP_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.strUdpOrRtpIp = strText;
            else if( 0 == QString::compare( strNodeName, UDP_RTP_PORT_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.nUdpOrRtpPort = nValue;
            else if( 0 == QString::compare( strNodeName, UDP_RTP_AUDIO_DELAY_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.nUdpOrRtpAudioDelay = nValue;
            else if( 0 == QString::compare( strNodeName, INPUT_VIDEO_FORMAT_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.nInputVideoFormat = nValue;
            else if( 0 == QString::compare( strNodeName, OUTPUT_VIDEO_FORMAT_NODE, Qt::CaseInsensitive ) )
                channelSettingInfo.nOutputVideoFormat = nValue;
        }

        // save to vector
        if( !IsExist( channelSettingInfo ) )
        {
            m_vecChannelSettingInfo.append( channelSettingInfo );
            m_vecNewChannelSettingInfo.append( channelSettingInfo );
        }
    }

    return true;
}

bool CParseChannelSettingConfig::IsValidIndex( unsigned int nChannelNum )
{
    if( m_vecChannelSettingInfo.isEmpty() )
    {
        qDebug() << "m_vecChannelSettingInfo is empty";
        return false;
    }

    if( (unsigned int)m_vecChannelSettingInfo.count() < nChannelNum )
    {
        qDebug() << "m_vecChannelSettingInfo 数组越界!!!";
        return false;
    }

    return true;
}
