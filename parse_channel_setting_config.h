#ifndef __PARSE_CHANNEL_SETTING_CONFIG_H__
#define __PARSE_CHANNEL_SETTING_CONFIG_H__

#include "qdom.h"
#include <QVector>
#include <QStringList>
#include "values.h"

typedef struct tagChannelSettingInfo
{
    unsigned int nChannelNum;        // channel number
    QUALITY_ID   enuQuality;         // Excellent/Good/Fair/Poor
    bool         bRtmpEnable1;        // 是否打开RTMP，true：打开， false：关闭
    QString      strRtmpAddr1;        // RTMP 地址
    unsigned int nRtmpAudioDelay1;    // RTMP audio 延迟
    bool         bRtmpEnable2;        // 是否打开RTMP，true：打开， false：关闭
    QString      strRtmpAddr2;        // RTMP 地址
    unsigned int nRtmpAudioDelay2;    // RTMP audio 延迟
    bool         bUdpOrRtpEnable;    // 是否打开UDP/RTP，true：打开， false：关闭
    bool         bUdpOrRtpAddr;       // true UDP,  false RTP
    QString      strUdpOrRtpIp;      // UDP/RTP ip
    unsigned int nUdpOrRtpPort;       // UDP/RTP port
    unsigned int nUdpOrRtpAudioDelay; // UdpOrRtp audio 延迟
    unsigned int nInputVideoFormat; // 0：1080I 50/60 1：720P 50/60
    unsigned int nOutputVideoFormat; // 0：1920x1080I 25/30 1: 1440X1080I 25/30 2: 720P 50/60 3:720P 25/30
} CHANNEL_SETTING_INFO,*PCHANNEL_SETTING_INFO;

class CParseChannelSettingConfig
{
public:
    CParseChannelSettingConfig();
    ~CParseChannelSettingConfig();

    /*!
     * \brief  实例化xml解析对象
     *
     * \return CParseChannelSettingConfig* xml解析对象
     */
    static CParseChannelSettingConfig* NewInstance();

    /*!
     * \brief  释放xml解析对象
     *
     * \return bool true/false 成功/失败
     */
    static bool DestoryInstance();

    /*!
     * \brief  获取指定channel number的频道设置信息
     *
     * param int[in] channel number
     * param CHANNEL_SETTING_INFO[out] 频道设置信息
     *
     * \return bool true/false 成功/失败
     */
    bool GetChannelSettingInfo( unsigned int nChannelNum, CHANNEL_SETTING_INFO* pChannelSettingInfo, bool bNew = false );

    QUALITY_ID GetChannelQuality( unsigned int nChannelNum );

    QString  GetChannelRtmpAddr( unsigned int nChannelNum, bool bRtmp1 );

    bool GetChannelRtmpEnable( unsigned int nChannelNum, bool bRtmp1 );
    bool GetChannelUdpOrRtpEnable( unsigned int nChannelNum );

    unsigned int GetChannelRtmpDelay( unsigned int nChannelNum, bool bRtmp1 );
    /*!
     * \brief  保存指定channel number的频道设置信息
     *
     * param CHANNEL_SETTING_INFO[in] 频道设置信息
     */
    void SaveChannelSettingInfo( CHANNEL_SETTING_INFO ChannelSettingInfo );

    bool IsChangedVideoFormat();

    void SaveChannelVideoFormat( QVector<INPUT_VIDEO_FORMAT> vecVideoFormat );

private:
    int Addref();
    int Release();
    bool ReadFile( QDomDocument& doc, const QString& fileName );
    bool IsExist( CHANNEL_SETTING_INFO NewChannelSettingInfo );
    bool SaveFile();
    bool ReadChannelSettingConfigFile();
    bool IsValidIndex( unsigned int nChannelNum  );

private:
    static CParseChannelSettingConfig*  m_spParseXmlConfig;
private:
    int                    m_nReference;
    QVector<CHANNEL_SETTING_INFO>  m_vecChannelSettingInfo;   // 从配置文件获取的视频信息
    QVector<CHANNEL_SETTING_INFO>  m_vecNewChannelSettingInfo;
};

#endif // __PARSE_CHANNEL_SETTING_CONFIG_H__
