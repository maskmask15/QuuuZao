/*
 * file_op.c
 *
 *  Created on: 2023婵°倗濮烽崕鎴﹀箯閿燂拷9闂備礁鎼悧鐐哄箯閿燂拷20闂備礁鎼崰娑㈠箯閿燂拷
 *      Author: Unityliu
 */


#include "file_op.h"
#include "xparameters_ps.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "shareOpConfig.h"
static FATFS fatfs;

u32 Init()
{
    FRESULT result;
    int status;
    TCHAR *Path = "0:/";
    result =f_mount(&fatfs,Path, 0);    //婵犵數鍋涢ˇ顓㈠礉瀹�鍕埞闁兼亽鍎抽埢鏃堟煥閻旇袚缂佸顦甸、鏃堝礋椤忓棛鐓戝┑鐐舵彧缁插墽鍒掑畝锟芥禍鎼侇敍閻愬弶妲梺缁樻礀閸婅崵绮堟径鎰骇闁冲搫鍊婚幊鍥煕閿濆洤鍘撮柟顖氬暣閺佹捇鏁撻敓锟�0闂備礁鎼�氼喖顪冮敓绲�0
	if(result){
	   printf("error : f_mount returned error \r\n");
	   return XST_FAILURE;
    }

    return XST_SUCCESS ;
}

//SD闂備礁鎲￠妵娑㈡晸閽樺鏆遍柡鍛倐閺屾盯骞嬮敐鍛呮捇鏌￠崱顓熷
u32 Write(char *FileName,u32 src_addr,u32 byte_len,u32 start_addr)
{
    FIL file;       //闂備礁鎼崐绋棵洪敐鍛瀻闁靛浚婢侀幏鐑芥偨濞堟寧鏁梺鑺ョ壄閹凤拷
    FRESULT result;
    UINT BytesWr;   //f_write闂備礁鎲￠崹鍏兼叏閵堝姹查柣鏂挎啞娴溿倝鏌￠崒娑橆嚋缂侊拷閿熶粙鎮楅悷鐗堝暈鐟滄澘鍟村畷顖炲箻鐠囪尙顦ч梺闈涳紡閸曨厽娈搁梺璇插缁嬫帡銆冩径瀣╃箚妞ゆ挶鍨归弸渚�鏌ㄩ悤鍌涘

    result = f_open(&file,FileName, FA_CREATE_ALWAYS|FA_WRITE);//闂備胶鎳撻悘姘跺箰閸濄儲顫曢柟瀵稿仧閳绘棃鏌ㄩ悢璇残撶紒瀣樀椤㈡棃宕ㄩ鎾呮嫹椤旀祹褰掓偑閸涱喖顏�,濠电姷顣介敓钘夊暱閿熻棄缍婇幃妯诲緞鐎ｎ兘鏋栭悗骞垮劚閹虫劙骞楅悩缁樼厱闁挎棁宕甸崢婊呯磼鏉堛劎绠炵�规洘鑹鹃埢搴ょ疀閹炬湹鐢婚柣搴ｅ仯閸婃稑顩奸妸褏鍗氶悗娑欘焽閳绘梹銇勮箛鎾村櫤闁诲寒鍠栭湁闁绘瑥鎳忕�氾拷
    if(result)
    {
    	printf("error :file:%s f_open returned error \r\n",FileName);
        return XST_FAILURE;
    }

    result = f_lseek(&file, start_addr);//缂傚倷绀侀ˇ鎶筋敋瑜庨幈銊╁煛閸涱厾顓奸梺瑙勫劤绾绢厾澹曟繝姘厽闁靛鍎遍顏堟煛鐎ｎ亜鏆炵紒鍌涘笧閹瑰嫭绗熼姘啅闂佽崵濮烽崕鎴犳閺囩偐鏋旈柟杈剧畱濡﹢鏌涢妷顖炴妞ゆ劒绮欓幃褰掑炊闁垮顏�/闂備礁鎲￠崝鏍偡閿旀拝鎷烽崹顐㈠摵闁哄矉绠撻弫鎾绘晸閿燂拷,闂備胶鍎甸弲婵嬧�﹂崼銏㈢閻庯綆鍋嗛々鏌ュ箹鏉堝墽纾挎繛鍫濇惈椤顦村┑顔炬暬閹儵鏁愭径濠勫摋闂佽鍨庨崟顔兼儓闂備礁鎼崐绋棵洪敐鍛瀻闁靛繈鍊栭崕宥夋煕閺囥劌澧伴柛妯荤懃閳藉骞欓崘褏鐩庣紓浣介哺閸ㄥ爼骞堥妸褝鎷烽敐鍌涙珖缂佸顕槐鎾诲礃閹勭亖闂佽法鍠愰弸濠氬箯閿燂拷
    if(result)
    {
        printf("error : f_lseek returned error \r\n");
        return XST_FAILURE;
    }

    result = f_write(&file,(void*) src_addr,byte_len,&BytesWr);//闂備礁鎲￠崝鏍偡閿曞倸鍑犻柛鎰ㄦ櫇椤╃兘鎮归崫鍕儓闁轰礁鐖奸幃妤呮儌閸涘﹤顏�
    if(result)
    {
        printf("error : f_write returned error \r\n");
        return XST_FAILURE;
    }

    result = f_close(&file);    //闂備胶顭堢换鎴炵箾婵犲伣娑氾拷娑欘焽閳绘棃鏌ㄩ悢璇残撶紒瀣樀椤㈡棃宕ㄩ鎾呮嫹椤旀祹褰掓偑閸涱喖顏�
    if(result){
        printf("error : f_close returned error \r\n");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

//SD闂備礁鎲￠ˇ褰掑Χ鎼粹剝顔戦梻浣告啞閸ㄥ吋鎱ㄩ妶澶婃辈闁跨噦鎷�
u32 Read(char *FileName,void * DestinationAddress,u32 ByteLength,void * start_addr)
{
    FIL file;    //闂備礁鎼崐绋棵洪敐鍛瀻闁靛浚婢侀幏鐑芥偨濞堟寧鏁梺鑺ョ壄閹凤拷
    FRESULT result;
    UINT BytesRd;    //f_read闂備礁鎲￠崹鍏兼叏閵堝姹查柣鏂挎啞娴溿倝鏌￠崒娑橆嚋缂侊拷閿熶粙鎮楃憴鍕憙閻忓浚浜、姘閺夋垹顦╅梺鎯х箺椤鎮鹃柆宥嗗�垫繛鎴炵懐濞堟瑥鈹戦埥鍡楀籍鐎殿噮鍋婇弫鎾绘晸閿燂拷

    result = f_open(&file,FileName,FA_READ);    //闂備胶鎳撻悘姘跺箰閸濄儲顫曢柟瀵稿仧閳绘棃鏌ㄩ悢璇残撶紒瀣樀椤㈡棃宕熼锝夋暘闂佽崵濮村ú鈺佺暦闂堟稈鏋旈柟杈剧畱濡﹢鏌涢妷顖炴妞ゆ劧鎷�
    if(result)
    {
        printf("error : f_open returned error: filename:%s resule:%d \r\n",FileName, result);
        return XST_FAILURE;
    }

    result = f_lseek(&file,start_addr);    //闂備胶鍎甸弲婵嬧�﹂崼銏㈢閻庯綆鍋嗛々鏌ュ箹鏉堝墽纾挎繛鍫濇惈椤顦村┑顔炬暬閹儵鏁愭径濠勫摋闂佽鍨庨崟顔兼儓闂備礁鎼崐绋棵洪敐鍛瀻闁靛繈鍊栭崕宥夋煕閺囥劌澧伴柛妯荤懃閳藉骞欓崘褏鐩庣紓浣介哺閸ㄥ爼骞堥妸褝鎷烽敐鍌涙珖缂佸顕槐鎾诲礃閹勭亖闂佽法鍠愰弸濠氬箯閿燂拷
    if(result)
    {
        printf("error : f_lseek returned error \r\n");
        return XST_FAILURE;
    }

    result = f_read(&file, DestinationAddress,ByteLength,&BytesRd);//闂佽崵濮村ú銊у垝椤栫偛鍑犻柛鎰ㄦ櫇椤╃兘鎮归崫鍕儓闁轰礁鐖奸幃妤呮儌閸涘﹤顏�
    if(result)
	{
		printf("error : f_read returned error \r\n");
		return XST_FAILURE;
	}

    result = f_close(&file);//闂備胶顭堢换鎴炵箾婵犲伣娑氾拷娑欘焽閳绘棃鏌ㄩ悢璇残撶紒瀣樀椤㈡棃宕ㄩ鎾呮嫹椤旀祹褰掓偑閸涱喖顏�
    if(result)
    {
        printf("error : f_close returned error \r\n");
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}
