/*
 * file_op.c
 *
 *  Created on: 2023婵犲痉鏉匡拷妤佹叏閻戣棄纾块柟杈剧畱缁狀垶鏌ㄩ悤鍌涘9闂傚倸鍊风粈渚�骞栭锔藉亱闁绘劕鎼粻顖炴煥閻曞倹瀚�20闂傚倸鍊风粈渚�骞栭锕�绐楁繛鎴欏灩缁狀垶鏌ㄩ悤鍌涘
 *      Author: Unityliu
 */


#include "file_op.h"
#include "xparameters_ps.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "shareOpConfig.h"
//#include "xscutimer.h"
#include "../xdm/dma.h"

#define DMA_DEV_ID          XPAR_AXIDMA_0_DEVICE_ID
#define RX_INTR_ID          XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID
#define TX_INTR_ID          XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID
#define INTC_DEVICE_ID      XPAR_SCUGIC_SINGLE_DEVICE_ID
//#define DDR_BASE_ADDR       0x00000000                      //0x00000000
//#define MEM_BASE_ADDR       (DDR_BASE_ADDR + 0x1100000)     //0x01100000
//#define TX_BUFFER_BASE      (MEM_BASE_ADDR + 0x00100000)    //0x01200000
//#define RX_BUFFER_BASE      (MEM_BASE_ADDR + 0x00300000)    //0x01400000
int net_init(void);
static FATFS fatfs;

//闂傚倸鍊风粈渚�骞夐敍鍕殰婵°倕鍟畷鏌ユ煕瀹�锟介崕鎴犵礊閺嶎厽鐓欓柣妤�鐗婄欢鑼磼閿熻棄鐣濋崟顒傚幐闂佸壊鍋掗崑鍛存倵閹惰姤鈷掑ù锝呮啞閸熺偞銇勯妸锕�濮嶆鐐茬箻閺佹捇鏁撻敓锟�
u32 Init()
{

	FRESULT result;
	int status;
	TCHAR *Path = "0:/";
	result =f_mount(&fatfs,Path, 0);    //濠电姷鏁搁崑娑⑺囬銏犵鐎癸拷閸曨偉鍩為梺鍏间航閸庢娊鍩㈤弮鍫熺叆闁绘棁顕ц缂備礁顑呴ˇ鐢搞�侀弮鍫濈妞ゅ繐妫涢悡鎴濃攽閻愯埖褰х紒鎻掑⒔閸掓帒鐣濋敓鑺ョ閹间緡鏁嶉柣鎰级濡差剟姊虹紒妯荤闁稿﹨宕电划鍫熷緞閹邦剛楠囬梺鍐叉惈閸婂骞婇崶顒佺厱闁挎繂娲ら崢鎾煙椤栨艾鏆ｉ柡浣规崌閺佹捇鏁撻敓锟�0闂傚倷绀侀幖顐わ拷姘煎枛椤啴鏁撶徊锟�0
	if(result){
	   printf("error : f_mount returned error \r\n");
	   return XST_FAILURE;
	}
//	return XST_SUCCESS ;



	DMA_Init(DeNoise_DMA_DEV_ID);
    return XST_SUCCESS ;
}

u32 Write(char *FileName,u32 src_addr,u32 byte_len,u32 start_addr)
{
    FIL file;       //闂傚倷绀侀幖顐﹀磹缁嬫５娲晲閸涱亝鐎婚梺闈涙禋濠線骞忛悜鑺ュ仺婵炲牊瀵ч弫顖炴⒑閼恒儳澹勯柟鍑ゆ嫹
    FRESULT result;
    UINT BytesWr;   //f_write闂傚倷绀侀幉锟犲垂閸忓吋鍙忛柕鍫濐槸濮规煡鏌ｉ弬鎸庡暈濞存嚎鍊濋弻锟犲磼濞戞﹩鍤嬬紓渚婃嫹闁跨喍绮欓幃妤呮偡閻楀牆鏆堥悷婊勬緲閸熸潙鐣烽鐐茬閻犲洩灏欓ˇ褔姊洪棃娑崇础闁告洦鍘藉▓鎼佹⒑鐠囨彃顒㈢紒瀣浮閵嗗啯寰勭�ｂ晝绠氬銈嗘尪閸ㄥ綊寮告笟锟介弻銊╂偆閸屾稑顏�

    result = f_open(&file,FileName, FA_CREATE_ALWAYS|FA_WRITE);//闂傚倷鑳堕幊鎾绘倶濮樿泛绠伴柛婵勫劜椤洟鏌熺�电浠ч柍缁樻閺屻劑鎮㈢拠娈嬫挾绱掔�ｎ亶妯�妞ゃ垺妫冨畷銊╊敊閹惧懏瀚规い鏃�绁硅ぐ鎺撳亼闁告侗鍠栭锟�,婵犵數濮烽。浠嬫晸閽樺鏆遍柨鐔绘缂嶅﹪骞冨Ο璇茬窞閻庯綆鍏橀弸鏍倵楠炲灝鍔氶柟铏姍楠炴鎮╃紒妯煎幈闂佹寧妫佸畷鐢稿储濠婂懐纾奸弶鍫涘妿缁犵偟锟借娲橀懝楣冨煝鎼淬倗鐤�闁圭偓婀归悽濠氭煟鎼达絽浠柛濠冪☉椤╁ジ濡歌閸楁岸鎮楀☉娆樼劷闁崇粯姊归妵鍕疀閹炬潙娅ら梺璇插瘨閸犳牠婀侀梺缁樼懃閹冲繒锟芥熬鎷�
    if(result)
    {
    	printf("error :file:%s f_open returned error \r\n",FileName);
        return XST_FAILURE;
    }

    result = f_lseek(&file, start_addr);//缂傚倸鍊风粈渚�藝閹剁瓔鏁嬬憸搴ㄥ箞閵娾晛鐓涢柛娑卞幘椤撳ジ姊虹憴鍕姢缁剧虎鍘炬竟鏇熺節濮橆厾鍘介梺闈涱焾閸庨亶顢旈鍫熺厸閻庯綆浜滈弳鐐电磼閸屾稑绗ч柟鐟板缁楃喖顢涘顓у晠闂備浇宕垫慨鐑藉磿閹寸姵顫曢柡鍥╁亹閺嬫棃鏌熸潏鍓х暠婵☆偅锕㈤弻娑㈠Ψ椤栫偞顎嶅銈嗗姃缁瑩骞冭ぐ鎺戠倞闂佸灝顑呴锟�/闂傚倷绀侀幉锟犲礉閺嶎厽鍋￠柨鏃�鎷濋幏鐑藉垂椤愩垹鎽甸梺鍝勭焿缁犳捇寮幘缁樻櫢闁跨噦鎷�,闂傚倷鑳堕崕鐢稿疾濠靛锟斤箓宕奸姀銏㈩槸闁诲函缍嗛崑鍡涖�呴弻銉ョ閺夊牆澧界壕鎸庣箾閸繃鎯堟い顐ｎ殙椤︽潙鈹戦鐐毈闁诡喓鍎甸弫鎰緞婵犲嫬鎽嬮梻浣筋嚃閸ㄥ酣宕熼鍏煎創闂傚倷绀侀幖顐﹀磹缁嬫５娲晲閸涱亝鐎婚梺闈涚箞閸婃牠宕曞澶嬬厱闁哄洢鍔屾晶浼存煕濡崵鎳冮柍钘夘樀楠炴瑩宕樿閻╁海绱撴担浠嬪摵闁搞劌鐖奸獮鍫ュΩ瑜濋幏鐑芥晲閸屾稒鐝栫紓浣割槸椤曨厾妲愰幘璇茬闁诡垎鍕簴闂備浇娉曢崰鎰板几婵犳艾绠柨鐕傛嫹
    if(result)
    {
        printf("error : f_lseek returned error \r\n");
        return XST_FAILURE;
    }

    result = f_write(&file,(void*) src_addr,byte_len,&BytesWr);//闂傚倷绀侀幉锟犲礉閺嶎厽鍋￠柨鏇炲�搁崙鐘绘煕閹般劍娅囨い鈺冨厴閹綊宕崟顒佸創闂佽桨绀侀悥濂稿箖濡ゅ懏鍎岄柛娑橈工椤忥拷
    if(result)
    {
        printf("error : f_write returned error \r\n");
        return XST_FAILURE;
    }

    result = f_close(&file);    //闂傚倷鑳堕…鍫㈡崲閹寸偟绠惧┑鐘蹭迹濞戞熬鎷峰☉娆樼劷闁崇粯妫冮弻銊╂偄鐠囨畫鎾剁磼鐎ｎ亶妯�妞ゃ垺妫冨畷銊╊敊閹惧懏瀚规い鏃�绁硅ぐ鎺撳亼闁告侗鍠栭锟�
    if(result){
        printf("error : f_close returned error \r\n");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

//SD闂傚倷绀侀幉锟犓囪ぐ鎺懳ч幖绮瑰墲椤旀垿姊绘担鍛婂暈闁搞劌鍚嬮幈銊╁Χ婢跺﹥杈堥梺璺ㄥ櫐閹凤拷
u32 Read(char *FileName,void * DestinationAddress,u32 ByteLength,void * start_addr)
{
    FIL file;    //闂傚倷绀侀幖顐﹀磹缁嬫５娲晲閸涱亝鐎婚梺闈涙禋濠線骞忛悜鑺ュ仺婵炲牊瀵ч弫顖炴⒑閼恒儳澹勯柟鍑ゆ嫹
    FRESULT result;
    UINT BytesRd;    //f_read闂傚倷绀侀幉锟犲垂閸忓吋鍙忛柕鍫濐槸濮规煡鏌ｉ弬鎸庡暈濞存嚎鍊濋弻锟犲磼濞戞﹩鍤嬬紓渚婃嫹闁跨喍绮欓幃妤冩喆閸曨厽鎲欓柣蹇撴禋娴滎亪銆佸顓烆嚤闁哄鍨归ˇ鈺呮⒑閹呯妞ゎ偄顦甸幃楣冩焼瀹ュ棗锟藉灚绻涢幋鐐垫噽婵炲牊鐟ラ埞鎴﹀煡閸℃绫嶉悗娈垮櫘閸嬪﹪寮幘缁樻櫢闁跨噦鎷�

    result = f_open(&file,FileName,FA_READ);    //闂傚倷鑳堕幊鎾绘倶濮樿泛绠伴柛婵勫劜椤洟鏌熺�电浠ч柍缁樻閺屻劑鎮㈢拠娈嬫挾绱掔�ｎ亶妯�妞ゃ垺妫冨畷鐔碱敃閿濆鏆橀梻浣藉吹婵潙煤閳轰胶鏆﹂梻鍫熺▓閺嬫棃鏌熸潏鍓х暠婵☆偅锕㈤弻娑㈠Ψ椤栫偞顎嶅銈嗗姧閹凤拷
    if(result)
    {
        printf("error : f_open returned error: filename:%s resule:%d \r\n",FileName, result);
        return XST_FAILURE;
    }

    result = f_lseek(&file,start_addr);    //闂傚倷鑳堕崕鐢稿疾濠靛锟斤箓宕奸姀銏㈩槸闁诲函缍嗛崑鍡涖�呴弻銉ョ閺夊牆澧界壕鎸庣箾閸繃鎯堟い顐ｎ殙椤︽潙鈹戦鐐毈闁诡喓鍎甸弫鎰緞婵犲嫬鎽嬮梻浣筋嚃閸ㄥ酣宕熼鍏煎創闂傚倷绀侀幖顐﹀磹缁嬫５娲晲閸涱亝鐎婚梺闈涚箞閸婃牠宕曞澶嬬厱闁哄洢鍔屾晶浼存煕濡崵鎳冮柍钘夘樀楠炴瑩宕樿閻╁海绱撴担浠嬪摵闁搞劌鐖奸獮鍫ュΩ瑜濋幏鐑芥晲閸屾稒鐝栫紓浣割槸椤曨厾妲愰幘璇茬闁诡垎鍕簴闂備浇娉曢崰鎰板几婵犳艾绠柨鐕傛嫹
    if(result)
    {
        printf("error : f_lseek returned error \r\n");
        return XST_FAILURE;
    }

    result = f_read(&file, DestinationAddress,ByteLength,&BytesRd);//闂備浇宕垫慨鏉懨洪妸褍鍨濇い鏍仜閸戠娀鏌涢幇銊︽珖妞も晝鍏橀幃褰掑传閸曨剚鍎撻梺杞扮閻栧ジ骞冨Δ鍛剬闁告稑锕ら锟�
    if(result)
	{
		printf("error : f_read returned error \r\n");
		return XST_FAILURE;
	}

    result = f_close(&file);//闂傚倷鑳堕…鍫㈡崲閹寸偟绠惧┑鐘蹭迹濞戞熬鎷峰☉娆樼劷闁崇粯妫冮弻銊╂偄鐠囨畫鎾剁磼鐎ｎ亶妯�妞ゃ垺妫冨畷銊╊敊閹惧懏瀚规い鏃�绁硅ぐ鎺撳亼闁告侗鍠栭锟�
    if(result)
    {
        printf("error : f_close returned error \r\n");
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}
