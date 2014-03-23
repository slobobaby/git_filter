/*
 * Copyright (C) 2014 Brian Murphy <brian@murphy.dk>
 *
 * This file is part of git_filter, distributed under the GNU GPL v2.
 * For full terms see the included COPYING file.
 *
 */

#define DEBUG 0
#define MALLOC_STATS 0

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#if MALLOC_STATS
#include <malloc.h>
#endif
#include <regex.h>

#include "git2.h"

#include "git_filter.h"
#include "dict.h"

#if 1
const char *oid_list[] = {
"5a5f4d49f8e2677c1a465bb0e479924131fd035b",
"46dc80b1788bbe69bfb8e50f138ca2cd45dcf3de",
"7695c3fbc425de6e016fd483c8eea95c31572272",
"c5ac28d3c2f23032dfd8408a1295ebcb41e39750",
"41b7ed9ddc2330fddd7062cabadf674c7fa3572f",
"4dce474bb09362b8836fd017e155887ae655cab1",
"2bb3b9722ce30037fb0424b7542b4a6c3d323ad3",
"402e552815bcdd03c404076095013523b4d8e4c0",
"1799ee8ae760ab3fdb36e3cc69d9e3bf57eff9a7",
"f59e671c06f9d32240b63ebda2da004870bd2936",
"ae46525f00cc7ce2c2585c58746ae8669cd42c14",
"b68b18d74af06f1fec6e7eb91a9cedd00a8e2fb4",
"135894e58b6639de02afeea2cd52a8eef4a08621",
"3315afe5fe066577b782c5894b81da974079d0dd",
"7be286164c097fd0c6d240f8bf808c49b3992063",
"5d26f60284b9ecde873f2885b778bbf27a57da85",
"7a256832d26dddd42ce410492abc8e9f7c6c9651",
"7da72c7d41a0c8926d8f45a61ecc1b55c96a899a",
"0ba118daedaebde47412a90ed5a495a1ac40c15e",
"ceb94a984773c178bf88416f9575fad8b156b7cd",
"79ec7aa62f256b038556a34fdb003874ad042600",
"439572369e60db05aef7b2e701602e8d99461005",
"d08160837b3b4c495a5a89481b3d57d15c8d46c3",
"e34a4d6187339108adfe17168a32a838b47012fe",
"bae171e19bdc56d048fb1d12c9022b6d520eb553",
"e352ed98d4783e0f94614fe0b30a285c5fb58ee5",
"ae4f607752ec728b6857548171867807bac1cbea",
"5ca54c2e2071f940dd94dd1c8776ccb13cfe1581",
"d5367c930d79d2015bf2bbbe6c89109b2e95412b",
"c64df52ff07df8dda6a8b6af42744ed003c1e834",
"65859090406ec4cf4ed4f96bb7f81c245bc62385",
"97c0dda37d76bf7bbaca81b8251a399e21ce3d93",
"d9269d4aebc22b62a9e8a242e62dec2931463ff8",
"c7d812b16e3b0d21c47efd94e1deaaefffd61f39",
"6a892ebc98e0ad6b3c745ddb0227c2dfa4ce2601",
"aab632a1ab7320fac3cadb9aabaec4e851e62bcf",
"e7cc787515b45289336ae6ccd183616c306e079e",
"c9cca48912e0843e43207fc1efe4eeed693ebea7",
"1d3a98b2dd10d1627a170e57c93a9fdaf483e7f0",
"7dfab4960ffc7577976dc2f38f77309b646979f5",
"68b61487a63f5c2af5cf0d4f8a78b5041c23a887",
"1ba77d03f86b6699a757e1f7e5502cebf97d0a45",
"a887258cef0487abd3583d24cc2538051855e2f9",
"01c20c683a5c1a452fd6b40dda27f1f75070ecf4",
"91d9edd66941c36a3c12137f02df46f7a0330f5d",
"8423e7670499adb59e8c77b245d3323e76f5a6b5",
"c0bb2d75765489a998880a4b44cd2bb274ac20c9",
"e11f51162b2d297779e463fab433570fb77690c3",
"2897ef6c5c62604682b0c2777fd6ef7e7c93597c",
"eeadd3aaf091d87b9926dc386129cb8f0196ac17",
"a0a506a7ee3522a9dab8b8c8571b3a8defbd9559",
"e8718b9f4447b7f7acdc929647bf0f10709d5fca",
"bb4ce7f1c6dfcfaa22b5105c8dffd6f0e79031dd",
"24ddcf12319d6fdac170afe4b7931b697ab702ab",
"a4343a3aab9afa162db3d0836fb6c737bec4d940",
"482dce7efdc616ddeb309ad1927d858c8b8aeadd",
"2f79bf3c5759100c43729ba948a38c7fd0fd2af7",
"feac9d2aa8ebcd2ea166bc9df9ce5e5ca4e090db",
"996272a669670c7766968be30a7ece9a676283aa",
"1d226a7001b159d7cc75084df6c31dfc73517cf5",
"948408de2fb60000e6c21390bb4bc62e8d849095",
"00d12b637a9f3e23337923e425146cf96aaf10b0",
"e898a0227685607f2856ec3bb5a2be4d1190bc1b",
"5ba00e7ce8c89755d1a6ac4c0fa1117947fb793a",
"3451399bbd2f9f8bef8d5e801330005082d0be4e",
"4408068639f01d0ab3c8dbaa172c60c816b5ace7",
"c51cd4595b1a9e6bd0e68b68fc9157a0171ced67",
"25caaeb4bcc65588bbf3b7a9a391cb85cb1d39ed",
"71918357f995237ee3a4f8d73c14f90d330b4e84",
"3c92303fa0b04ad1d5e73961833b66e6c908b585",
"13ae88660168ca24dd8447c53d28ca6f1a413d51",
"7aed5eb5baf93553c7821e35d3acc8089b902f7f",
"c63e0c115b9b0c449c6331b31d90bcf93ad381fc",
"0354e27d63c38beb907551432f9322e7ff741214",
"0616117196f1f9ce24b7717d13ebac138700acfc",
"f6a9448acb20a5a840091597e1ed86ff84115944",
"f6732544aa55b6fbbd20ea26bb86c35436dcbcca",
"ce0d440d4a5e5e0cb22aeda8ffa4ec785bb25beb",
"1e04d9f68e0ff4ad22f0d7af5b70ab0bd59cbff8",
"ef8d9f70aec935c5e0c898d92f2b18da7cad8b22",
"ee59d39cd4af491d86c5e3f59100a1735e7d8756",
"e06149fd36aab3455da5d9b4d481355715e47d2d",
"f7bc45bcb20d83f696b9e032c6042a9654cb24fb",
"4cc68b3655a5d21062ac9525219ab8a74255120e",
"8e57e5f304a9f7690f841882a3904d8116c703f8",
"3cb35323414ca6ac9080d7fa1008a3c61b585520",
"acdbcde6bddd21809a4cf39cdaa3ee37b3c28282",
"ff9de263b64aed0fd6e0155eeaa546e234b2b07c",
"6b831de23e96ea5a668c40183c1a76007dfaa7c0",
"dc17796764e8c8d8a29b19c9b09dade66350e2b3",
"6a7acb578f0db4ede36b60bbae761a5b428b21f5",
"cf6e9ba5e58ee89bfe422de6386c93cab5e0eb71",
"f5388fa764d0a62a04e9110cdb83b0e6e99a2440",
"53991ca7a8fa7743ce2ffaa010f273a8f93e9210",
"c756944d4d82c291cd8736684b0db587c451204a",
"0df732db5066a355e9fb50a6e3f64379faf38d6f",
"d4b5d097987d9cd5af1b3bb6174a017ff71304fc",
"78446360574aedbfdf6e3eb29d3c28a32c57f2db",
"350fe7d1b0b554190e128bd4cfb858420540f5f7",
"393ae857c546ea484814c17a0f9b1912f0790fc3",
"b0acc1782becfff34085fd84c2eb80c37a6b0ccb",
"b5a888c3e54378a2b243e10effc237db36261a24",
"5a2ca9dea878580552136b0a19d75f9d9de6f4ac",
"166ca1f4bd34d459a6be308490ed444aa9a49a44",
"d8b36c1379e96d2de69d3059ba94f23e40b2fc5e",
"7a51480b9eacb2592f74171d216720ff65e66e88",
"8c5ea8fb4e7bde534cfd25d2ec4df72032bdfe16",
"f24b06f90e0594421f135f81ed9eb80b0bda33f7",
"bd18da7fe390b6dffaad9b53b91a75f784d82c08",
"c0bb5618bb3fa6358646d844a7ae656c64abb59b",
"55a9389a5ce42ed90b055dacb75cd621aa72f773",
"282cdcfd58dd8b43468b7f2559fa78a07ba913aa",
"19d6f2d2f9915a4558e895c435b6962ae74bfc1a",
"7d040bd381fdb22b740b6726dd0ef09ba25ed945",
"be66cbcc73c191deb831354f6c2ff2698c65493e",
"0b86e623a96154b538346f2ba647df5ec4bd1533",
"b3e4b6856d9f8ff6426840133141300321e06fa5",
"3c5f65e454982719a8dcc37c98039bd0e05a4432",
"f3b739e84b1405fa70775b78709e6d79b141f4e1",
"376d7be72604a8c3654d492eb5eff63b237e0689",
"96a79b0ad9f0c7d175a94e69e48e6eddb4201119",
"598c1575f1f67041336a5c67cea32c4b22769843",
"7146eb98811412673ab3ff44816325670600ab62",
"fd2af78838ded4cbf45344cde61fbaf637479d58",
"e27b91f20d946171bc004c21c20e08f6da39af16",
"c3aeb804292efb7d3bd2663c8e2b38590fd294cb",
"851cea1a6841bb89574cf1145606ccf0f0fee03d",
"36fd645527da8c9f91b5ce34dadda1396a4ad205",
"194419c5a6d48e2e2cbea844cc347ae05c12dd85",
"6eed92e6743db9c1a415e982bf7081258fdb16a5",
"8f533c68d6f8afb08359060d878338776beb9769",
"8b70a9b90c88e68f243dc6e3aa1d510d43ac6adf",
"7ea705ed630c68f3beb91351627ea4ffac0822b1",
"db384adbb41ac7afd98205106a4ae18e7ef0158f",
"8fffcb3f328f6f3131339422ca2035280e7afe04",
"534b4ae681df41481717b2a9c9cae8941d1c4156",
"140b15d32587452091e3121d1cd033232b076290",
"f31506db2e10149d6733289a5d44dd0500d16bdd",
"c0ec80ceca375e32dd4bbc69fee6ee783985cfea",
"76c6541bec6f221cea99fe9d4a62f38ba447d126",
"720b80161b31dbd147a3afdcbf29ebce168fba3c",
"15aac0a6ada264293bb3714703152b02ba93e197",
"79107643da0668600b750670882946dde4a36053",
"f381b448675949fae4805c67f044a09858e86a28",
"bf925a32fe6497473a8d0dce15c34488d8d14bac",
"1f7a24919574c7dba5ddfb5261b4b8a0d294ba04",
"d4b5c04352b9913cbfb79d620a7b5ddc393c3bb4",
"ee513db63d613a9f347a311b0fa3b13062b1cfae",
"936751827ab96f9cf7d11b616a8cb84bf352b051",
"51df3f14047940de9d6d05479ea637e5f55dc55a",
"6f8511f506f422f64ca5df2a16b66bfe69e531e5",
"a25714954fb5550f97c888b4329d9b9db7ab20d3",
"a1ef7123faa18b859d510460d7b2ca259cb1b846",
"7621c17288cbffe9126766a5be5e577a659287c1",
"30a600df000a9a586f5283b9c350a0b5d37e68a4",
"d23d200450cdfaf1e6bc64ff0565bbda908661ff",
"21996aa870249698059465352357a7e408b61ef9",
"e1dc29fc1b47b29cf9260007d359b3a17ab985f7",
"53d5f6bc02c0692ecc24d612733b6ea36f10c1c5",
"ec6c05173dfd105c215547e69f1a670f12451acd",
"e2a485121858bd6f2a51d89d6cbd41c898f51fcf",
"7d48c11a0e3170bc9f42d15ab260adaaac8a46fc",
"3b79b7cde4a9b32bfbb93ec70524151ab0e24ce8",
"d54fb221a109212a86a5bcb6666353019175f03f",
"ef20ca346cc6aa35e0d6367d50277acaeb2606a5",
"cc5e5d7848f9b10b32d8792ba3a4bbf3e163e508",
"37b6992c0deed114a6f9e37362398395b7faf901",
"10d0bb8960de7496dcf5e8c5bde079715ebce4b8",
"ab3a690e24e6988dabef916eea39d6e9586f571e",
"e8ece8af0b86d3f6d6bc7b1e0768af2083433038",
"6fc506e0fb90f971492747348c721c06ad3ac099",
"7da8219e88e5c5a31557916885004dc491a5dc99",
"7bc5e83892e1237d40ce9be9de80b9310f5b7891",
"916d7f788935cf1c02df502741385226135a7b64",
"63a4cf6ce50f7c198e0573b8c6728a03b0cc215d",
"50b373a059cab1fe1300be17f9943c182caf9347",
"615bf63b1a7d5370aa853572bca76c1a5a912ef8",
"751b313d39d2ec3945d73baaa0943761de3086c5",
"eeb91e5bd2b68afa4df37a3491280b10afdb2bbf",
"781d030baef9d03994d35ce77b7092b00eff08ff",
"d356b3a0609d82e24e182ac1dc8e8b44da93c57a",
"642873149af331d8357b056b17c505d05ca27357",
"cca1ba012bd197259a572085ae1004d1ba76de4a",
"233558dcd5e7cce2c602234bf9bc87b608148b11",
"dd77042222d8477ce2c113b4cb4c37888aa856f3",
"ce313fbf7ad0b6868a867048ab7e29d025ca0f00",
"6d3cec6348764a3c12d794b0ab3dc81bc5886f32",
"589e8e851de13948fc891fb8b8773c85adc8e3e3",
"3986560621b878174b76d67999ea5b29724af908",
"02667e5018eae1e8cf0ef7c0f4c5686f7ad89d5a",
"461fc6afa47788e848210d1c85035f6996929ec1",
"ab95cc6e5857d4918c445052459c930788ea7b82",
"3972b2e0b43ac1f32791957a9e1ed163e82c5224",
"6d0331410e9fada1f818268e0d39771279f9d82f",
"fbc0e172db94785b02e710a0377bc52c9342077f",
"2fbd360f82930424ce882b8c16e3d94a6d416c70",
"0aba4687930f0b4b96d94596ba61c4236f4b3adf",
"79548b9cc48ef712feb5f10647176f94ecdefbe3",
"9fa49e139926e2e139a77e6a9aa0e90105a04af6",
"970eaef5b3f78f16f4d69d46b03e03724c84ec2b",
"f30a6b84479bab9db353158481587f6a7be29ff6",
"027b5339ed55d3b400d41f1ccd6b1cbdb0450577",
"e99f99dfb11407416620721160c98b9b6e81bf1b",
"9ce22c3cf4a3f648509146fcdcff434529dc31f0",
"1cc29db99d358062c809410ad21e552cd3a8177a",
"f238b334b3129c5fba6ea7fde4a43211d795cd32",
"1e192b4606a7a6d91e3517eac8fc0243e6d36c5e",
"242e03885a96b3d7a47dee0ac83fee69ba229cbe",
"a74dd5a4bfc5b862cf5fc66d1c3dfdadc6ce66b6",
"561fcca2517f03f2486bf0c316177ba8958a6d77",
"0219954baeb8a52c6864d867de18c61413d3a35a",
"c5e2699d2847205f36fcfc2308af1c00fc285eac",
"0f6ce119dafdb8d42fd3778a8f95e7584e7d4d6d",
"03403efab4a67b02b2003ddfc8efdfff33ce079e",
"8d510b3777d3f50c53c37d04fcc10a975a8a67d7",
"d9e4ac2f10123a0e4096c6b9551bb5b39aaa03fc",
"bdc8906eec64a9ad216989a5f2fe7eb67ccbe6d9",
"269ba04888f46833152a47e4d558c418c110728a",
"e1070d1e4974f6f381d59c81ccd7da0d8a91cebc",
"45999a5c0716ccadb4d7e63fba7961c5da11cd56",
"2794b29a9c60deb156b850c8ce997fe03c78b74d",
"879102f493c624b9a84c62a7ab19d960237d2420",
"b5f1cd2b589a9f6362d7b63e68d109388342dbcd",
"6c0f8df2524b01ee3e33f1a0bc68881f905df506",
"f2c502e842722a16d53dd5f7a80446b9339ef2c6",
"5893b1781f3aaeeb832978448729a4692bafe69e",
"0f79e2be8164b09b55a65d6e6403071a8f2ee333",
"9696f74e6015e58ae89efcad6cc7710d714272b5",
"fc3b3ed8d5ca9c6689873c20b161f24c9e8da0e9",
"3479e408c4e6cb49a2abbba54885df3d2da4b54e",
"c8bc39d637261cdb6c2b5c4247816d68faf329d0",
"29a5c3e592d1fbc4d88b6188386d4f1dffaf7c62",
"04239277dbe24ac27f666ed56da02863573530c5",
"b1d3ea315df698b6b67d66c98f810f21256904a0",
"9e6c6cee8604926ce4d156ee7691e4b7ddd0fe25",
"9d64e00f2b60549a869c44dca0a0c29f7f58dc7c",
"0c4e407461cda60aad73ab06f8a36bd406a01d45",
"0568f206de2b60c5a4350dbe01f5d0c79710d91b",
"8acf18545c83da8f97efec43cf3ce6a755ad0b4b",
"11dab500350bc8adaf7eb47176bfbb9a32638108",
"6c69ea8b0005adbfe541edd1d0cd080e20b9434d",
"d63a59cd2508834fd2c7424d3b2e59873caa59c7",
"3a39b1d3c5ab5f0071e60aa8eba1ebbfc19d024c",
"ff6ad984a4a83bdc195075567c071366cc618726",
"7d1edc5d6854054b563784303cba819593f96eb9",
"e090b5bb2502024b9cf1c3e338b073ce7f9cc053",
"ed2606eef4b560473ead9e138e21d2f8f0634e1c",
"0442c146e2de00c4e8efd228646621ce993a88cc",
"fc70a2a7b5978c3100f26f21b412f0b126bcaf18",
"b926f353f028f8e7713fb6df637359fb1430349d",
"8896cdf4b79ee9f849ac151f4bd0ae1253139a68",
"3b4a21982ac87d0797142c45214940ac72727371",
"de164befdb42843a7db2726a8bd18dbaee4a4bff",
"7365205e583dbe3f6546911b16bdd0b4a23997c1",
"a0c63e43cb9d0a4beff9872af5bea8619664e32d",
"e81341b49fc3eba3a5e6ba36cb092ef7825c20d4",
"2dc511dabfddd3d36f6a7bbf81bf56962a3d7e67",
"da382d1f6eaf95fdce7b6ac4615d29e3c8f03839",
"419049308e60022fa87c3e7280953f9cf756724b",
"6906496f9ff53e57db3dd34c891e75a72478ad28",
"9d4d4a3f934014233b017559ad3678507416862f",
"f2d5a8877354527de944259bf81e82daa20540a9",
"20b972dfa122e343ffcbfcbf6787d9795cb43bc3",
"67d7f8dd19b50eda814356bd98182388a6e475ee",
"d3d53555413c49827355e6f302996e89af91c5a9",
"d8782a4ddd439bea0cdb1180fe53bdc2134786c7",
"85d2dd206b89afecfdb643d7f648e3e52cb3606e",
"ed335a575d5c89b17b30f0d9ffd13919d22da56a",
"dea62bde07d6c13512c3a34dc3d90b634662e94a",
"1fe9361ba691b6483be76ee7a92eb490b5cbb122",
"8deaf6b619885e5fcbe54a67c7339a15058b33b6",
"00f68dccb214c59e27504a08ffad42b162535e10",
"457e7e99d58340a82cdce19f2beaa860026fa95c",
"c21c820cbe9d4e20a5e0acd9dbe2c6774e1f0a82",
"0655a37ad8dee882c81bcfc4cef17e13137d7eb2",
"d84133facf3eee675f2d0cf9575832e3774c4d83",
"93f53b67e12ad8c23177c1ee6c903f79b3729f7b",
"c5f6cf70d819d264c390a1856667fa86aa1dce8d",
"779408388de7c1647a03266f47326b842130fa2c",
"f8290b0ac4ecc2b1c887d28288d1e0c23ceb5aa2",
"1587aa9ef73c0a51e548117b7ca7a39af26371d3",
"634a868d863868185f93bfaeab8c7a8b90b7a253",
"0bd6107125ebfdb6069ac6d6febfe45357810b2d",
"dfe2f97359f7f3743d152e72eb7b82c16179be84",
"c34a14ef8b89d105d8c840e7373ba9996d1da3c8",
"68abbbc71f9c50ffd4b1cc4d99aa7a45105306b2",
"936fbed424af7ccc42a392e16e661a7f9d88f56c",
"3f1da7c8242fe8c6751d910846662b3db1540436",
"89a7926f88c058dd7a6796adcd80165042fb936e",
"0f939fec87b92ae975ddcf7df537560e3e282db3",
"1729be44074b7ecaaf0b278a0aa4d29a6f15bb52",
"776efe679e34576afff40d552e00cd8707515d16",
"0c8eb9324049da6e34b9a4d5e65c8c4670aa2f49",
"259adb9d1dd381323fd4dcaaebbe532a98bc9a5e",
"694113eb5d0a0f80d098441c3ac0ca9701e5b2cb",
"ccd69f7c6ab5287b8a6fd82816e9fa0e0d8cc93c",
"50874b9ff36034b5f4d16a173475dcbea2018599",
"b6ed358075ba0ebbe530649ce92ba6651a1f74d0",
"335a0d71bb54e672cda7c35acefe7cc93944dfb4",
"0b365bb278a11e66304623aaf53928fc7796c8e3",
"022b5395feac51fec8d9e48ca3a67dd61c8b7943",
"19c4f5b0af7e5dba71a9127436d66480f31be697",
"f41c7cac900bd92b8623fd93757371db63004660",
"5c56d4cab2ca3a63ed606e6e3f08d22df682e6c1",
"7e2b8869b7478c4d62b32f649b23a2cc195c89b3",
"f8bd09a42f0d22362c8f86695e1449b8200035b5",
"e192f4f5a45f4e8c78ccf3846d6ac03c8e7da257",
"52fd80534c52f5b0f4d9ccd79b77c44455747a98",
"60bfb0033ee87474ff8694b29928c8c328ebfaf2",
"48748695759841d2298a84cb6801e02e95b1d6b1",
"a60d9431a429ca393914dcfcc24cbce5833a9596",
"9b9936587f4c79962bef3b6c86a77c3d0ea36a70",
"5558472b7594976e1fb97e5f5e3285a303755662",
"35d30cb9161124a4cc8a8c4c7704f7478d0ccb66",
"ebd78968aea9a34469b3b8befece891cb2d5c820",
"ee169ac3bb28062b4a1b0358739e952d394bad8d",
"c8d84cb4863e00b5040aa60a1826277a66a1831f",
"54a092e372fc81988dfa692930705002bd76afa4",
"bbb94f9fa6e632f9c18993411faf748887826cef",
"51fe62579ec45cc6be19102c22621dca85a2c9a2",
"be0397adf02a81b2b29bdf907e044e93c01705ba",
"225ca02f67b6cb53ed6d38c4337e9d811910d7ec",
"53aa254d33387722edebe23292fc7ca27c8a6bba",
"d7d43d7742121386a2880811484e480180d8888e",
"60c133c272dd2d5b4dbe9b5bb02181a78e978d8b",
"4e6156995ffdf4079d94deb91b0be7c1d925c880",
"3d5075d1bc5d30478e0afffc563f8980b8826ca5",
"fe62b2be302f45056102e73679ab422ef1003620",
"b49712bcb621093839f83651ba2e12921d061a8a",
"f59fa192abbb36b4dc7e1d9bb302e3750fbd436b",
"69c9f73eef7dcf722220d3324de9637fe1882c35",
"c341d2f4dce27bc515ae27687ac5d98cc643de50",
"a8f747acf1b1b84f05948eba797f1ae9eb3ba335",
"59251ba22ef94ca8d86a475ae641761292637d77",
"1c29ed714034f3bb5c6fcd42aaf698b20d17c8d5",
"e2be3fa07974b76f418b99d394cd9e2e3798ca01",
"b5e703498c26e3a18aaa9b404259a97e9210a5bd",
"44026e3f55f4cb5a8ba671ba4d1ab8e85bb8466a",
"31ed40a8c4a7589553a90cd9eb2507304b90b77c",
"fa1348e17cde8c90f7eeab1d29994dc7a642ff8c",
"9569e6ec86a422dc5fb947e3bcbbed03a74bdc2b",
"57b8f284592fa3085134e6c8a4fb44e418a764bb",
"f90122a28180f3deedcf910b70c39625b43812e7",
"0e3a6ee514ff6ad66ad3c23304f0066ac05db223",
"202dac7916cc4ba5e86cf0e84dded99b9d19ca42",
"c60d4f15daf21099ae93c38110f2578806ffd012",
"1479786a90db2a7762a1b9164a76aa9581519ab7",
"c3392f3bc103a0eb58383a20c26ab080c91c87ed",
"9d4f6ec9b0636c90da6ccd38b391f1efddc36559",
"8cc6372e835fc52f75c84613d659a6cde4de0670",
"a3bb81b89de71e7a498422243c243aa7e9dba89b",
"f5a23879d91a98ea254390fab68008c1b7eba26f",
"7bfc305737403790338dc7edacf60eaed0dd327b",
"30522319f6847b6bcc43f194e2419c85e6414815",
"8a11cc85671b0d4031164f005ae71f0db258cb40",
"2a4156118bf05ad89762e88ca20d1370cb695dcd",
"e0a82672815414ecf7af528ff9c45c57f370ade9",
"7d38773b790b6d3d95e7956ce7598b6907387c41",
"635a336addd811d45ce372037ce55551073e8c6a",
"d557c99975a624c4be98219a8a1048af6f4c5130",
"0a2aa5c39eb7064933cb14ad8eb492cbd4fe7fb5",
"c3c8ef2d99d93ea5dec5557a58df9619dcf6d0e7",
"0487738c1eac740d55cb898dd0e31affa567714a",
"c3501f769f3f924d7e27e94dc6a5c3343d77b7d8",
"bc26b170c8c81c8b26607eea31c61af552b77344",
"05340ac14831c99bdf2a8cc61ad5a5ea37ee0360",
"c21b0509a5930162b4243e064a756092b0a39495",
"01ba6accd393c118a5b52639f3927c0bfc6199bc",
"9722b68c4d66c5768689b2aacb3544eef8ce5778",
"dad79af1e1928e9048277910ebc512636d593652",
"235bad42697c79f4d9a3843b6df4a24f62fb7736",
"ceba6db2796639c917cfcd760fb0410345e23f42",
"f91998eb43efd25f1c65aa64c0578b8c19dd1900",
"28c0930639b7be1a5916d5a729dabf26dee55582",
"5f9244df2386efc9c48235a22378b287a95aa4e3",
"588143d7925ef28d98ec24fdcd1f3865b6db9e5e",
"0c7971e6038068770c5a2dd3b70e0284accb382a",
"cbf84d5f3ab3112ba92d87abd38f25b761a90a85",
"e6a8db41acf12d02acdfb197eb3cfd2cce198d20",
"a08522ef40fed62d819d1576eb0cd7b2df2ed763",
"61bcfa6a196b2527baedfe2423013a1c42f6c9b8",
"2a13a6c889345d712bb6da3a2052dc06e6203441",
"5b06fd5e09d869270981db8957acf19831b65937",
"c501da056e4cbc525f86fef68caa8302fda577fb",
"e0f493b88a78238e3e2f9df45abf7c591d5ee804",
"f16b3f4669ffd487b5816a04f085f3c99c834b4e",
"072d6e825fd1272d5c15930b08cc4ab177cb32e7",
"98710c89183313ecdcdf5276a653a8c8de6ee9b0",
"7968cf09af155e6fcd3359a8dd68b6c8e9b8c454",
"870f1d37de16709d59f054e3f09929f9f3f03e2d",
"62cbb5d135eee9f6b351bee6e015b77ad6498a59",
"abe376a6dc3b207635d9c9caddf42a19532e8a5b",
"d753c73a1be8321ad715d0c63e71c1d1028aaa3b",
"4a7409b224d52efb2b89f16c56bad99ad5a3b16c",
"f67f6dcb1a7af5292b9298a45084188cc1f97f9c",
"1cf1be5afb0bbafb90dd8bc52d1a3bf77b6dab1e",
"e25183567b535a5b0ba096498b1c450bfeec6941",
"660c7da7dc3977defce4b989434037cb09e9d148",
"dff2e9a0dadb523c9bb6e65ae159c8af394c55dd",
"357e507eb43c292eed072550afdb916c2506de08",
"4b9a49993d77d4f7cb606b8756362275ca177191",
"f3a49b05a2ee44c3954d23a2ddc16f717de0662d",
"a290c1dfed2f0424ccc823f059c646488341d539",
"09610aee216cc91ecb861ffcb556458edd760f26",
"e1065cac06d76fc3476061177880107062c14144",
"fcab091517595523f62d5d1df6dc7133ee36a70e",
"5305698e54b73cab89e7eabf9cd10799d74504ea",
"a6b946b6ebbc916eb8f2dca019c9a4c6d4ef6a7c",
"573b0d58e3fc78e4eb52ecc7e09d1cf4ea60eed7",
"0af5ad00c4cb9dcf199da83c32f24c97498dbdfc",
"5841a171cd15b9ab2fa349e7ba25983f14b3482d",
"e3dcdd371368c09773f8f3c976222ec970de7403",
"b15d3a0dea2b22c8328f18897650d0156416a1dd",
"f8917632b3053a8071c58b3af1d633206f07492b",
"6fd87a57fe7430757444c0ad8f7dc5164205a7f0",
"35aa95ec3b346dfd853c75a1e74021d5e268e5a1",
"91c5822939f25654994b1a2ab2c743971023d75c",
"28068295b7a2b577320d14d7c3efc3dfdefe7964",
"d541709d559dc49b3902670e76408c009fc41761",
"24d184d4d6312034f8d165d8f23b66067548ed80",
"111f52efb4d50d0295fd5584f6792b39cc629a2f",
"eeec623c2cab52d6db74a9ade53120f90a015d7d",
"43562995caa876679c43616ff9bded509eb11949",
"bcc04d80abc0237a9e60566c1848cc6a512358b2",
"cff906c38f7d85d4eb539c32a5fe189d5f9e9ae7",
"4c33e38ef943571e6f339844a82f01fc7be1685b",
"ebd6836548748f2bbeae749e1068663b4af772d1",
"5e005a801abc3b474c45ab5ba23eb9f915502782",
"2702529878350f8bb172b57871079a7ad67892c8",
"947b401598451feaecd81be1693cbb6f45249788",
"3d9d146a1e3f0abfa452640b18c3bc8a390385e6",
"2104814361a95252c905df0dda8ca38036d0b4ba",
"5ae4fc972005b3bcc9d4d74d63fd4f66d7f649da",
"dee1719631a308b56234f232bd24dd026c23454c",
"999103092e633fe3e029eef95858fb697993bc9f",
"a304330f430193a53cddee4a7b01c63fdebf1b2e",
"6a79bccb775848d38eaa5e81521ad243dc0ca9a6",
"bcff65ea710b107799afec2c82c3e8075d07c8b3",
"3d79c829d77eed400e1f918fc835b3be30b1c8aa",
"d1798e111a8b1f6a8dcc93542d4a9f8a7512538b",
"ce47c2079673a6d7367aa292746c66a0e552a1b7",
"a0a71ca6be325b234ab69779990cb9af4f60e328",
"391c692a83bd001f363e5cf4b82584ac4e2a7c3a",
"be10d279f11b77fc71c8225bc66acf61f2ad930a",
"94a7c4976a4b6a629315d2330094e09cbee597cd",
"6cc4c54b8b90012b0b6a5abff05e7401d247200d",
"baea4fbaba1ea74273a06f7cdcc1f5d2d51d95cd",
"79ee58eb8ec2bc295b9468e0c96284992cfd3368",
"cc366e9c491c8b6e36016d4e854a560e792a7a9f",
"6bc9ce0489c9e3f855672c17a65a38cbc3d1277c",
"e84716b0debee72e8ee64b08b23bce8270a25239",
"ecdb5242a8640747845d762a2023791b582dff19",
"ff2b357a87513ab162a8de7efe2c133d06be7cb0",
"486378cc1be6b310832d119ef5a39709767c5561",
"0ef4f1b478f3fac3cf612c91afea461c719e2211",
"950fb661154dd4a2f5882f935f46641518b1d380",
"60721e9f05c8cb73d11fd7068f8fcd98a5a590a3",
"296ea5fe37b38fdb05ae22a3412e80115c25aeb2",
"3146a6f50f22cc35dd8bd5354f3f3e77f128ea78",
"8411a483ff0e218976dfaa5a036a9d71b6937309",
"0f082f59fde06975ab13de2860fdf04973769333",
"fdadcefa76d59516cc186714840e43447385c7ba",
"db81035fef1f02175bc2dcb11f56415c14bdbae1",
"70d44341172fad880232f6d634d7264b056e3dad",
"fe383a1b0fc545bc11b8985236edb8d0d40db81d",
"c49b9fc80b8b5fab19042d687ee80c5c18d73226",
"db939a08a842c31ca8c7822d5bfa41858b5062b6",
"418beffb100929afb66f0b4894c26164c0aa8430",
"8ecd669907e9c76c4b6ece529bde617e713b482b",
"f9cf2205a71585ceb995264662ac0c1ecde201ba",
"2ddaf6ce2386abe03d2917709d73c047ccf20b71",
"b2c17143e19b3493edbefd6e55803509c3864c10",
"131df7d40c0f8d494b323acc18e9dcc63cbd1b10",
"e98a114c84ac032c4b3f4aa68f0f8f0983e3e8cf",
"4f6e63a089ea1f3faddd0977f68c05d76537d4da",
"b9920b5f017fdff520d8e3bde5d522345eda4de2",
"15e3646b520a07878d4039940f44c2936e991f0d",
"8b0f5cac7f23a27e47e0811abebf7530e7a2ce22",
"9994e3873e0d04a2b1b8c64537b7affc7663041a",
"db98e0ec0802157bd7e2a8e63279a922bbcfcf2c",
"239c0c767cb2cfb7bbb16997437781a190d45b3e",
"1e872367318fb8b29d2b3f0b5559a87c92dad355",
"b0724394949070401f2ec17cf716fc0a3b4670a0",
"21bfbf533b5eb5228e5c0a11a005b3360771a8de",
"39474de889a70378abcd2967ba450ea5d618a337",
"e11019fec9f6dcd7b14aa81174733756631240d5",
"f0bef4898a6317b6aab96394599db4fe6d441d7d",
"c697b4538067b066697324d821634429fce891e4",
"98d6a316ee34763a9b2cb104352b9afc7f543cee",
"0b322586f27f269465d87bed9a1f143310f85eb7",
"c90c91f2cdcd91a152aeef04aed38adbd89767f6",
"1ce9c4069c65144fa8d621ea4963a20db71b02bf",
"dcf2d8a86addd17c96512030fdbc45c11b2e6cc9",
"e4d3449b42d94730b864582973d91cb86acfcd8a",
"e821c3bbe5272271249c529d00253d7b7c5b779d",
"4b314b5d8cdf1f0ea74976eb946bb6d96ea03e6d",
"b8e7c56023ebdb6598ca797034696281e54b2c31",
"717ccb4b30d2344b93c2454ad34ea5aa915be8f9",
"660dd4a409647c901980ce6fef384bc67ed46f60",
"6a78897da7230f21c93fd12c75097d31d9d4da47",
"a81228d446e7aeb39dd437d9325930e774b8bc20",
"42556ebd44003ff114d5553181c6ebb798a8cb76",
"876f73656f4d815b7b6646c69e688b6f691d2f45",
"b2d988440d1b9e51d5d7aa7cda25d5157956898a",
"6f5212de46fa5cba7e4ae2c37b19b2807e602417",
"4891f886dd9d36461c6d1f4e9abf5b3ff669c2c6",
"46ae2cd50a8e1581b40594ab92553e143ec18134",
"a4c2bc24b4536df8b85f6172ab0817670af6417f",
"745110549b55c596e3d236038256b60f184b3b97",
"f6e15922212fe2f6ae55a41ffb619d6dc2a8d217",
"50e9589d319a67a17bd698f0f6754ad8f58f456a",
"575fead9bf25c8a48ef1296d26413f47aee976a6",
"4aa5dba857664ebb1017f8c91e35aa88aca3bab5",
"72f73a40bd55298c6052834a46a135c35c8a1e13",
"570541b4e6f097be05523012fc52e6287e12c616",
"626c6d8a9ed25bad740132a549a0748dabc8a509",
"629f08e64ffd37a18afa09a1c7e7cf1dd7dcdf63",
"653399aa9ced117962f2822d2ba378fa6eb1b878",
"0c41bc46e9d6ad3d2101510f9b688475a3224d58",
"83dca04461920dcb25c3474e85b52e3d49ba2ec3",
"703aa89c3e06428a94eecb71a84c287db8bab165",
"67c41a8a8dc8f969263f6fc7fe50aad815dac321",
"3470bec83e910bfbcfd7173852023e57aad83301",
"3b161711e0728c8a85a0df7a89dced9ff47b1094",
"07c88dfb7c5ac131bf10d6a80f360e7a5c587702",
"0228d4a2f0c17a57f42986be9591f169b8eee06f",
"74c86006a0ed056f10865de9e45ddcf08ed3b6c2",
"93cd21ce2c0db5f43ab26cc827bcd4fc12bf9994",
"aabe29606cc13545e7d72c767116edb191248f53",
"7df0f7b5daa1660b0a7255967774cd194ec4dc34",
"ee5df4b9d31857b0c7f37f152bebc2f8a5b39d10",
"9497e61cbd6d22368729cf3454d3ea68633f6fd2",
"4d4453f720aa31096dfaaab6ff45b10d0ae3774a",
"92d8b099a97198587f9412cda685c0a44545e7ac",
"a1f377b01c0f43f87d5d05eab10741f7690069dc",
"9a2e310f0293c3bf7fa0ba0484e5ee66b72f6734",
"17ea895fbbc78be43766c5744b2a571afb3bf293",
"200b7384572b261f10cb32e0d4bf86284ec65148",
"dc3f704c791503e1fe0e575f02c61e7e4be3f93f",
"bc0e542344c747e32a9584b77278f0548b75fc75",
"8806d51c55a9b0cd55059eca52c54afc8319ef96",
"abe1d5fcbc8629db4976fbb817231ea0f4d6f48e",
"4a00e9bb06718019960697dfd8742fdb63a1b5eb",
"0e5b18a3e0642bf1739efb64006f07b586c61d31",
"ab56a19ed8f359b70083032628569c2aa168063f",
"071bf8f6df964c3284ef6958db4bee1d6d9c94ff",
"a488c561f5664ee81d4f8d8cf58a7abd1b2dd46c",
"3cdc5f2dd820247990fad25450b6a90fa6ad4ebc",
"9b2e066ef863a76d19c2d72466466678e5768daa",
"38bca5e64e8d2a4c70cd4e6c3d610cb9f8d68cd3",
"b46d4c57d543fdbc023d4c5a4d7c1eff259ec3c1",
"e7235b265912777978ed953f05125ea148fab492",
"2340357cc9444ff1cc4a4de50fa25c094a97853d",
"58c089197b32ff9ca76946288f6866f095ea408d",
"b5e94378ec212b5b86413fa9c73ecf7c3edd82fe",
"b4f0aa1ab4595b2dccc877cee6bb1417cf39e157",
"f325050707a456c4cb1957834bc96d239b0d8134",
"28a97966c24b7e3ab82ae2fce02414ee004e69d1",
"014e78b4e4877e83ee5cd9796d5ed79f3e7909d4",
"3f78b3d7f80b05e0be03cb9dfdea2c7f838c3274",
"ddbc127138f0606af96c4fcbf14c227dcd1da569",
"77b62cd1979b40f2b304658b1be2c54180038c52",
"324ac3cfbbeb1337aaeb60e0b3027fb94b79b91f",
"65e5d7cd9fb61ffd33be4b109654434096239428",
"592a1e49e36c150e4f10b4013146c52889ce8244",
"0d30278f4faf00ec0ca644ff83c0417fa2540634",
"82053d84a75abb9b7366146f6caee8a72d2803f5",
"ba4e83a9aff6e435b0334afaa121db52bfecf0ef",
"a4503e82164fbcff1549f83fada7f43cfe6b699d",
"49a2216235396fff4688b6e27c66f136be66f594",
"9339ae7601ad905c5cbfc22ba9808e775156516b",
"0ed111cce1d50411d3cd9aceba1ff0d107e0b1e5",
"9d0e5522e71609f20496b9268a43ccdaf4ce80eb",
"a1296f7fe5f1c91625fd96cb516ff434ca8d5f99",
"5b10c86740431fcb674d1db17e477ef2b037484e",
"86e387ae232dbca31e4eedb07519bef840f28327",
"61c52dfd9871cb6f966bdd2247afa38dc8d40466",
"26aafe26d20897ce306c8363bda786966a700ab3",
"f2783dcf355333b02d4f0e4c64c75c04a7238610",
"cbe53bc5a1bc8108d7cd3096cdb37769605ec147",
"df90dce381476df8ccab29d4b755b646a65a2901",
"c98f86290513f9574691aa6a3635b8f68a2d82fe",
"62b250ca08861b7e35b8652e8be6af2cfc065014",
"ee5742b6fefbf39829e62e1cf42b224c09256cc1",
"16f37b8f49dcb9ee496edbd7b3b9319b4cc41383",
"e327f54b04d2ed6c0c55cbdd4ff002572c659ea3",
"a1d2be23bb4e0895bc1654e327fe53796ea862f2",
"662daf4c3a731af6846eac3666f73bb3e01bc7a5",
"f84f2ff5de82efb9f4fe8c911adbacd81345866a",
"6446b9dbe25391152a82010c51a2a80d8a56dbbe",
"2d8beb4a66300dc34e336503becb047ad46a4e01",
"f94f6e25243df1ed61b49b44162d3a5014303c6b",
"f79484166f93e613ef8d357df65b82ca3341b4af",
"a47ed367b00c02b2bcfeacfdb27417165ba7cacf",
"210e54bd420466e55cfc0e229b15aa11d65b413a",
"03e04b41580d82120129d36e995722dde3a9d0a1",
"69dcabb992160ea99a24d39f61eaed1112ef9b64",
"187c47562a7cfed32ea1a6182cf97e484bdd93d0",
"f55894afcf75a8685fc816e3cc19fe63c4c791d1",
"780bcb8842619e6daae2d4d4b63a8c20e4a831b4",
"02d196df18bf83f69dbb269d43d724b8a72c65ab",
"e5e62d81d7be83d2d535b3023b34b4edeabb4f0f",
"1d353df17bb7f911492c620893faf4a668ed2a16",
"c58c04b4e5abbb3d9a39c1a334e9839e8e3260df",
"f3734e36c7d8efbd0b26fb631bb403e0b718bb30",
"ef84d1a1046471bcbf55ca4c17667a6cdec447cd",
"5ef602ac98a2aeef186092667d5be726a4c25756",
"a1ef67d9a41c80a18044cd46fe406971d6418ed0",
"22af9f72e666ae5a6e79998ae7a19baf717d20d3",
"e49d509820bdac9d2b7f418c6678a811cd28bd6c",
"aac7394245e13631ced363f8756117a9ab6ff1d6",
"06df1502bd7d2d7e3afcccf4ef128a495860713a",
"2f80cd4cbf001c7e30b63acf629cc9434833aba1",
"37d34c9e4f81798c13391390f669863b38874a07",
"0c4cb20a4176b03a002476fb0b8d739734f0f5a6",
"b27aed614d849347d61bc938ef7fc1fd459ea187",
"b65f0eeed7af588c90afb7f9894102e9959e05c8",
"c73bae5b7bdbd6a0ed638fcda1cb8f10e0d3a9a0",
"96a3d45cdff7cf6b2ccd885e58c2b4802711d9b8",
"307b991914a396ebc1e053f194e11738c1f2f46e",
"9791876910251f67245ee0df1c8cb273934c28db",
"ab97018290a8dddd30030fffdbb4728b303e452e",
"ae8b2dbb9dcdf27b41f5b05db7b4c48b24a06043",
"3b7cd63936d0420a367218f7d927f492ea09beb3",
"8f74aa2a1571adab33b007981b145713a1d70e71",
"029ce24ad1b6387cb92573ee324f87a65417fa96",
"62a12410ff21ac67f9534d6931ebc34584e49236",
};

#define LIST_LEN (sizeof(oid_list)/sizeof(oid_list[0]))

static git_oid search_oid[LIST_LEN];
#endif

#define STACK_CHUNKS 32
#define INCLUDE_CHUNKS 1024
#define TAG_INFO_CHUNKS 128
#define TF_LIST_CHUNKS 32
#define CL_CHUNKS 16
#define BUFLEN 128

#define C(git2_call) do { \
    int _error = git2_call; \
    if (_error < 0) \
    { \
        const git_error *e = giterr_last(); \
        log("error %d at %d calling %s\n", _error, __LINE__, #git2_call); \
        log("%d: %s\n", e->klass, e->message); \
        exit(1); \
    } \
} while(0)

#define tree_equal(tree1, tree2) git_oid_equal(git_tree_id(tree1), \
            git_tree_id(tree2))

typedef struct _include_dirs {
    char **dirs;
    unsigned int alloc;
    unsigned int len;
} include_dirs_t;

typedef struct _tree_filter {
    const char *name;;
    const char *include_file;
    include_dirs_t id;

    git_oid last;

    git_repository *repo;
    dict_t *revdict;

    dict_t *deleted_merges;
    dict_t *deleted_commits;

    char first;
} tree_filter_t;

typedef struct _rev_info_dump_t
{
    FILE *f;
    const char *prefix;
} rev_info_dump_t;

typedef struct _dirstack_item_t {
    git_treebuilder *tb;
    char *name;
} dirstack_item_t;

typedef struct _dirstack_t {
    dirstack_item_t *item;
    unsigned int alloc;
    unsigned int depth;
    git_repository *repo;
} dirstack_t;

typedef struct _commit_list_t
{
    const git_commit **list;
    unsigned int len;
    unsigned int alloc;
} commit_list_t;

static char *git_repo_name = 0;
static char *git_repo_suffix = "";
static char *git_tag_prefix = 0;
static char *rev_type = 0;
static char *rev_string = 0;
static char continue_run = 0;

static unsigned int tf_len = 0;
static tree_filter_t *tf_list;
static unsigned int tf_list_alloc = 0;

static void tf_list_new(const char *name, const char *file)
{
    if (tf_len >= tf_list_alloc)
    {
        tf_list_alloc += TF_LIST_CHUNKS;
        tf_list = realloc(tf_list, tf_list_alloc *
                sizeof(tree_filter_t));
        memset(&tf_list[tf_list_alloc - TF_LIST_CHUNKS], 0,
                TF_LIST_CHUNKS * sizeof(tree_filter_t));
    }

    tf_list[tf_len].name = name;
    tf_list[tf_len].include_file = file;

    tf_len ++;
}

char *local_sprintf(const char *format, ...)
{
    va_list ap;
    int cnt;
    char *out = malloc(BUFLEN);

    A(out == 0, "no memory");

    va_start(ap, format);
    cnt = vsnprintf(out, BUFLEN, format, ap);
    va_end(ap);

    if (cnt > BUFLEN)
    {
        cnt ++;
        out = realloc(out, cnt);
        A(out == 0, "no memory");

        va_start(ap, format);
        vsnprintf(out, cnt, format, ap);
        va_end(ap);
    }

    return out;
}

static char *savefile(const char *name, const char *suffix)
{
    return local_sprintf("%s/%s%s%s%s",
            git_repo_name, git_repo_suffix, git_tag_prefix, name, suffix);
}

char *local_fgets(FILE *f)
{
    char *line = 0;
    char *r;
    unsigned int len = BUFLEN;
    size_t slen = 0;

    do {
        line = realloc(line, len);
        A(line == 0, "no memory");

        r = fgets(&line[slen], len-slen, f);
        if (!r)
        {
            if (slen != 0)
                break;
            free(line);
            return 0;
        }
        slen = strlen(line);
        len += BUFLEN;
    } while(line[slen-1] != '\n');

    line[slen - 1] = 0;
    return line;
}

void _rev_info_dump(void *d, const git_oid *k, const void *v)
{
    rev_info_dump_t *ri = (rev_info_dump_t *)d;
    char oids1[GIT_OID_HEXSZ+1];
    char oids2[GIT_OID_HEXSZ+1];
    const git_oid *o = (const git_oid *)k;
    const git_oid *cid = git_commit_id((const git_commit *)v);

    fprintf(ri->f, "%s%s %s\n", ri->prefix,
            git_oid_tostr(oids1, GIT_OID_HEXSZ+1, o),
            git_oid_tostr(oids2, GIT_OID_HEXSZ+1, cid)
           );
}

void rev_info_dump(tree_filter_t *tf)
{
    FILE *f;
    rev_info_dump_t ri;
    char *full_path = savefile(tf->name, ".revinfo");

    f = fopen(full_path, "w");
    if (!f)
        die("Cannot open %s.\n", full_path);

    ri.f = f;
    ri.prefix = "r:";
    dict_dump(tf->revdict, _rev_info_dump, &ri);
    ri.prefix = "m:";
    dict_dump(tf->deleted_merges, _rev_info_dump, &ri);
    ri.prefix = "d:";
    dict_dump(tf->deleted_commits, _rev_info_dump, &ri);

    fclose(f);

    free(full_path);
}

/* string sorting callback for qsort */
int sort_string(const void *a, const void *b)
{
    const char **stra = (const char **)a;
    const char **strb = (const char **)b;

    return strcmp(*stra, *strb);
}

void include_dirs_init(include_dirs_t *id, const char *file)
{
    FILE *f;
    int i;

    id->dirs = malloc(sizeof (char *) * INCLUDE_CHUNKS);
    id->alloc = INCLUDE_CHUNKS;
    id->len = 0;

    f = fopen(file, "r");
    if (!f)
        die("Cannot open %s.\n", file);

    while(!feof(f))
    {
        char *e = local_fgets(f);
        if (!e)
            break;
        if (id->len == id->alloc)
        {
            id->alloc += INCLUDE_CHUNKS;
            id->dirs = realloc(id->dirs,
                    id->alloc * sizeof (char *));
        }

        id->dirs[id->len] = e;
        id->len++;
    }

    fclose(f);

    qsort(id->dirs, id->len, sizeof(char *), sort_string);

    for (i=1; i<id->len; i++)
    {
        if (!strcmp(id->dirs[i], id->dirs[i-1]))
            die("%s: duplicate entries for '%s'", file, id->dirs[i]);

        unsigned int cmplen = strlen(id->dirs[i-1]);
        if (!strncmp(id->dirs[i], id->dirs[i-1], cmplen)
                && id->dirs[i][cmplen] == '/')
            die("%s: '%s' is a subdir of '%s'",
                    file, id->dirs[i], id->dirs[i-1]);
    }
}

static void save_last_commit(git_oid *commit_id, const char *filename)
{
        char oids[GIT_OID_HEXSZ+1];

        FILE *f = fopen(filename, "w");
        if (!f)
        {
            printf("WARNING: could not write last commit id file %s", filename);
            return;
        }

        fprintf(f, "%s\n", git_oid_tostr(oids, GIT_OID_HEXSZ+1, commit_id));

        fclose(f);
}


static void read_last_commit(git_oid *commit_id, const char *filename)
{
        FILE *f = fopen(filename, "r");
        if (!f)
            die("Could not open last commit id file %s", filename);

        char *e = local_fgets(f);
        if (!e)
            die("Could not read last commit id file %s", filename);

        C(git_oid_fromstr(commit_id, e));

        free(e);

        fclose(f);
}


static unsigned int read_revinfo(
        dict_t *revdict, dict_t *deleted_merges, dict_t *deleted_commits,
        git_repository *repo, const char *filename)
{
    unsigned int lineno = 0;
    FILE *f = fopen(filename, "r");
    if (!f)
        die("Could not open %s", filename);

    for (;;)
    {
        char *e = local_fgets(f);
        char *v;
        char *k;
        git_oid coid;
        git_commit *commit;

        if (e == 0)
            break;

        lineno ++;

        k = e + 2;
        v = strstr(e, " ");
        if (!v)
            die("could not parse line %d of %s", lineno, filename);
        v += 1;

        git_oid oida;

        if (git_oid_fromstr(&oida, k) != 0)
            die("could not parse line %d of %s", lineno, filename);

        if (git_oid_fromstr(&coid, v) != 0)
            die("could not parse line %d of %s", lineno, filename);

        C(git_commit_lookup(&commit, repo, &coid));

        switch (e[0])
        {
            case 'r':
                dict_add(revdict, &oida, commit);
                break;
            case 'm':
                dict_add(deleted_merges, &oida, commit);
                break;
            case 'd':
                dict_add(deleted_commits, &oida, commit);
                break;
            default:
                die("illegal entry at line %d of %s", lineno, filename);
        }

        free(e);
    }

    fclose(f);

    return lineno;
}

void tree_filter_init(tree_filter_t *tf, git_repository *repo)
{
    int count = 0;
    include_dirs_init(&tf->id, tf->include_file);

    tf->repo = repo;

    tf->revdict = dict_init();

    tf->deleted_merges = dict_init();

    tf->deleted_commits = dict_init();

    if (continue_run)
    {
        char *full_path = savefile(tf->name, ".revinfo");
        count = read_revinfo(tf->revdict, tf->deleted_merges,
                       tf->deleted_commits, repo, full_path);
        free(full_path);
    }

    if (count == 0)
        tf->first = 1;
}

void tree_filter_fini(tree_filter_t *tf)
{
}

dirstack_item_t *stack_get_item(dirstack_t *stack, int level)
{
    if (stack->alloc <= level)
    {
        stack->alloc += STACK_CHUNKS;

        stack->item = realloc(stack->item,
                stack->alloc * sizeof(dirstack_item_t));
        A(stack->item == 0, "no memory");

        memset(&stack->item[stack->alloc-STACK_CHUNKS], 0,
                STACK_CHUNKS * sizeof(dirstack_item_t));
    }
    return &stack->item[level];
}

void _stack_close_to(dirstack_t *stack, unsigned int level)
{
    unsigned int i;

    for (i = stack->depth - 1; i >= level; i--)
    {
        dirstack_item_t *cur = stack_get_item(stack, i);
        dirstack_item_t *prev = stack_get_item(stack, i-1);
        git_oid new_oid;

        C(git_treebuilder_write(&new_oid, stack->repo, cur->tb));
        git_treebuilder_free(cur->tb);

        C(git_treebuilder_insert(0, prev->tb, cur->name,
                    &new_oid, GIT_FILEMODE_TREE));

        free(cur->name);
        cur->name = 0;
        cur->tb = 0;
    }

    stack->depth = level;
}

void _handle_stack(dirstack_t *stack, char **path_c, unsigned int len)
{
    dirstack_item_t *s;
    unsigned int level;

    if (len == 0)
        return;

    for (level = 1; level <= len; level++)
    {
        s = stack_get_item(stack, level);

        if (!s->name)
        {
            s->name = strdup(path_c[level-1]);
            C(git_treebuilder_create(&s->tb, 0));
            stack->depth = level + 1;
            continue;
        }

        if (!strcmp(s->name, path_c[level-1]))
            continue;

        A(s->tb == 0, "stack overflow\n");

        _stack_close_to(stack, level);

        A(stack->depth != level, "stack error");
        A(s->name != 0, "stack error");
        A(s->tb != 0, "stack error");

        C(git_treebuilder_create(&s->tb, 0));
        s->name = strdup(path_c[level-1]);
        stack->depth = level + 1;
    }
}

void stack_open(dirstack_t *stack, git_repository *repo)
{
    dirstack_item_t *di;
    memset(stack, 0, sizeof(*stack));

    di = stack_get_item(stack, 0);

    C(git_treebuilder_create(&di->tb, 0));
    stack->depth = 1;
    stack->repo = repo;
}


#define add_pathc(p, item) do { \
    p[cnt++] = last; \
    if (cnt == path_size) \
    { \
        *p += STACK_CHUNKS; \
        p = realloc(p, path_size * sizeof(char *)); \
        A(p == 0, "no memory"); \
    } \
} while(0)


/* modifies path */
unsigned int split_path(char ***path_sp, char *path)
{
    char *next;
    char *last = path;
    unsigned int cnt;
    unsigned int path_size = STACK_CHUNKS;
    char **p = malloc(path_size * sizeof(char *));
    A(p == 0, "no memory");

    cnt = 0;
    while ((next = strchr(last, '/')))
    {
        *next = 0;

        add_pathc(p, last);

        last = next + 1;
    }

    add_pathc(p, last);

    p[cnt] = 0;

    *path_sp = p;

    return cnt;
}

void stack_add(dirstack_t *stack, const char *path, 
        const git_tree_entry *ent)
{
    const char *name = git_tree_entry_name(ent);
    const git_oid *t_oid = git_tree_entry_id(ent);
    const git_filemode_t t_fm = git_tree_entry_filemode(ent);

    char *tmppath = strdup(path);
    char **path_sp;
    dirstack_item_t *di;

    unsigned int cnt = split_path(&path_sp, tmppath);

    path_sp[cnt-1] = 0;

    if (cnt > 1)
        _handle_stack(stack, path_sp, cnt - 1);

    di = stack_get_item(stack, cnt-1);

    C(git_treebuilder_insert(0, di->tb, name, t_oid, t_fm));

    free(path_sp);
    free(tmppath);
}

int stack_close(dirstack_t *stack, git_oid *new_oid)
{
    dirstack_item_t *di;

    _stack_close_to(stack, 1);

    di = stack_get_item(stack, 0);

    C(git_treebuilder_write(new_oid, stack->repo, di->tb));
    git_treebuilder_free(di->tb);

    free(stack->item);

    return 0;
}

#if 0
git_tree *filtered_tree(include_dirs *id,
        git_tree *tree, git_repository *repo)
{
    git_tree *new_tree;
    git_oid new_oid;
    int i;
    dirstack_t stack;

    stack_open(&stack, repo);

    for(i=0; i<id->len; i++)
    {
        int error;
        git_tree_entry *out;
        const char *path = id->dirs[i];

        error = git_tree_entry_bypath(&out, tree, path);

        if (error == 0)
        {
            stack_add(&stack, path, out);
            git_tree_entry_free(out);
        }
    }

    C(stack_close(&stack, &new_oid));

    C(git_tree_lookup(&new_tree, repo, &new_oid));

    return new_tree;
}
#else
#define MAX_REGEX 1
struct filter_data_t {
    dirstack_t *stack;
    regex_t regex[MAX_REGEX];
    unsigned int regex_len;
};

typedef struct _s2_e_t
{
    git_tree *tree;
    size_t idx;
    size_t count;
    size_t change_count;
    const char *entname;
    git_treebuilder *tb;
} s2_e_t;

#define STACK_MAX 512
typedef struct _s2_t
{
    s2_e_t stack[STACK_MAX];
    unsigned int depth;
} s2_t;

static void copy_entry(git_treebuilder *tb, git_tree *tree, size_t idx)
{
        const git_tree_entry *e = git_tree_entry_byindex(tree, idx);
        const git_oid *t_oid = git_tree_entry_id(e);
        const git_filemode_t t_fm = git_tree_entry_filemode(e);
        const char *n = git_tree_entry_name(e);

        C(git_treebuilder_insert(0, tb, n, t_oid, t_fm));
}

git_tree *tree_walk(git_tree *tree, git_repository *repo)
{
    size_t count;
    size_t i;
    s2_t st;
    git_oid new_tree_id;
    size_t idx = 0;
    git_treebuilder *tb = 0;
    const char *entname = 0;
    size_t change_count = 0;

    memset(&st, 0, sizeof(st));
    tb = 0;

    count = git_tree_entrycount(tree);
    for (;;)
    {
start:
        for (i = idx; i < count; i++)
        {
            const git_tree_entry *e = git_tree_entry_byindex(tree, i);
            const git_oid *t_oid = git_tree_entry_id(e);
            git_otype type = git_tree_entry_type(e);
            const char *n = git_tree_entry_name(e);

            if (type == GIT_OBJ_TREE)
            {
                s2_e_t *st_ent = &st.stack[st.depth];

                st_ent->tb = tb;
                st_ent->entname = n;
                st_ent->tree = tree;
                st_ent->idx = i + 1;
                st_ent->count = count;
                st_ent->change_count = change_count;

                st.depth ++;

                C(git_tree_lookup(&tree, repo, t_oid));

                count = git_tree_entrycount(tree);
                idx = 0;
                change_count = 0;
                tb = 0;

                goto start;
            }

            if (type == GIT_OBJ_BLOB)
            {
                int x,m;

                m = 0;

                for (x = 0; x < LIST_LEN; x++)
                {
                    if (!git_oid_cmp(&search_oid[x], t_oid))
                    {
                        m = 1;
                        break;
                    }
                }
                if (m)
                {
                    size_t j;
                    char oids[GIT_OID_HEXSZ+1];

                    fprintf(stdout, "remove oid %s ", git_oid_tostr(oids, GIT_OID_HEXSZ+1, t_oid));
                    for (j=0; j < st.depth; j++)
                    {
                        fprintf(stdout, "%s/", st.stack[j].entname);
                    }
                    fprintf(stdout, "%s\n", n);
                    return 0;
                    change_count ++;
                    if (!tb)
                    {
                        C(git_treebuilder_create(&tb, 0));
                        for (j=0; j < i; j++)
                        {
                            copy_entry(tb, tree, j);
                        }
                        if (st.depth > 0)
                        {
                            for (j=0; j < st.depth; j++)
                            {
                                if (!st.stack[j].tb)
                                {
                                    size_t k;
                                    C(git_treebuilder_create(&st.stack[j].tb, 0));
                                    for (k=0; k < st.stack[j].idx - 1; k++)
                                    {
                                        copy_entry(st.stack[j].tb,
                                                st.stack[j].tree, k);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (tb)
                        copy_entry(tb, tree, i);
                }
            }
        }

        int new_commit = 0;
        if (change_count)
        {
            /* close current directory */
            C(git_treebuilder_write(&new_tree_id, repo, tb));
            git_treebuilder_free(tb);
            tb = 0;
            new_commit = 1;
        }

        if (st.depth == 0)
            break;

        git_tree_free(tree);
        tree = 0;

        s2_e_t *st_ent = &st.stack[st.depth];

        memset(st_ent, 0, sizeof(s2_e_t));

        /* pop stack */
        st.depth --;

        st_ent = &st.stack[st.depth];

        tree = st_ent->tree;
        idx = st_ent->idx;
        count = st_ent->count;
        tb = st_ent->tb;
        entname = st_ent->entname;

        if (change_count)
            change_count = st_ent->change_count + 1;
        else
            change_count = st_ent->change_count;
            
        if (new_commit)
        {
            C(git_treebuilder_insert(0, tb, entname,
                        &new_tree_id, GIT_FILEMODE_TREE));
        }
        else
        {
            if (tb)
                copy_entry(tb, tree, idx-1);
        }
    }

    if (change_count)
    {
        git_tree *new_tree;
        git_tree_lookup(&new_tree, repo, &new_tree_id);

        return new_tree;
    }
    else
        return tree;
}

git_tree *filtered_tree(struct include_dirs *id,
        git_tree *tree, git_repository *repo)
{
    git_tree *new_tree;

    new_tree = tree_walk(tree, repo);

    return new_tree;
}
#endif

void commit_list_init(commit_list_t *cl)
{
    memset(cl, 0, sizeof(*cl));
}

void commit_list_add(commit_list_t *cl, const git_commit *c)
{
    unsigned int i;

    for (i = 0; i < cl->len; i++)
    {
        if (!git_oid_cmp(git_commit_id(c), git_commit_id(cl->list[i])))
                return;
    }
    if (cl->len == cl->alloc)
    {
        cl->list = realloc(cl->list,
                (cl->alloc + CL_CHUNKS) * sizeof(git_commit *));
        cl->alloc += CL_CHUNKS;
        A(cl->list == 0, "no memory");
    }
    cl->list[cl->len] = c;
    cl->len ++;
}

void commit_list_free(commit_list_t *cl)
{
    free(cl->list);
    cl->list = 0;
}

/* helper function to find object id in one of the dictionaries */
static const git_commit *dict_lookup_all(const git_oid *id,
        dict_t *oid_dict, dict_t *deleted_merges, dict_t *deleted_commits)
{
    const git_commit *newc = dict_lookup(oid_dict, id);
    if (newc != 0)
        return newc;
    newc = dict_lookup(deleted_merges, id);
    if (newc != 0)
        return newc;
    newc = dict_lookup(deleted_commits, id);
    return newc;
}


/* find the parents of the original commit and
   map them to new commits */
void find_new_parents(git_commit *old, dict_t *oid_dict, 
        dict_t *deleted_merges, dict_t *deleted_commits,
               commit_list_t *commit_list)
{
    int cpcount;

    if (old == 0)
        return;

    cpcount = git_commit_parentcount(old);

    if (cpcount)
    {
        /* find parents */
        unsigned int n;
        for (n = 0; n < cpcount; n++)
        {
            git_commit *old_parent;
            const git_oid *old_pid;
            const git_commit *newc;

            C(git_commit_parent(&old_parent, old, n));
            old_pid = git_commit_id(old_parent);

            newc = dict_lookup_all(old_pid, oid_dict,
                    deleted_merges, deleted_commits);
            if (newc == 0)
                find_new_parents(old_parent, oid_dict,
                        deleted_merges, deleted_commits,
                        commit_list);
            else
                commit_list_add(commit_list, newc);

            git_commit_free(old_parent);
        }
    }
}

int parent_of(git_repository *repo, const git_oid *aid, const git_oid *oid)
{
    unsigned int cpcount;
    git_commit *commit;
#if DEBUG
    char oids1[GIT_OID_HEXSZ+1];
    char oids2[GIT_OID_HEXSZ+1];
#endif

    dlog("%s parent_of %s\n",
            git_oid_tostr(oids1, GIT_OID_HEXSZ+1, aid),
            git_oid_tostr(oids2, GIT_OID_HEXSZ+1, oid));

    C(git_commit_lookup(&commit, repo, oid));

    for(;;)
    {
        git_commit *parent = 0;
        unsigned int i;

        cpcount = git_commit_parentcount(commit);
        if (cpcount == 0)
        {
            git_commit_free(commit);
            return 0;
        }

        dlog("    %d parents\n", cpcount);
        for (i = 1; i < cpcount; i++)
        {
            dlog("    index %d\n", i);
            C(git_commit_parent(&parent, commit, i));

            oid = git_commit_id(parent);
            dlog("    parent %s\n", git_oid_tostr(oids1, GIT_OID_HEXSZ+1, oid));

            if (!git_oid_cmp(oid, aid))
            {
                dlog("    yes!\n");
                git_commit_free(commit);
                git_commit_free(parent);
                return 1;
            }
            git_commit_free(parent);
        }

        C(git_commit_parent(&parent, commit, 0));

        oid = git_commit_id(parent);
        dlog("    parent %s\n", git_oid_tostr(oids1, GIT_OID_HEXSZ+1, oid));

        if (!git_oid_cmp(oid, aid))
        {
            dlog("    yes!\n");
            git_commit_free(commit);
            git_commit_free(parent);
            return 1;
        }

        git_commit_free(commit);
        commit = parent;
    }
}


void create_commit(tree_filter_t *tf, git_tree *tree,
        git_commit *commit, const git_oid *commit_id)
{
    git_tree *new_tree;
    git_oid new_commit_id;
    const char *message;
    const git_signature *committer;
    const git_signature *author;
    commit_list_t commit_list;
    git_commit *new_commit;

    new_tree = filtered_tree(&tf->id, tree, tf->repo);

    if (new_tree == 0)
        return;

    if (git_tree_entrycount(new_tree) == 0)
    {
        if (new_tree != tree)
            git_tree_free(new_tree);
        return;
    }

    commit_list_init(&commit_list);
    if (tf->first)
        tf->first = 0;
    else
        find_new_parents(commit, tf->revdict, tf->deleted_merges,
                        tf->deleted_commits, &commit_list);

    /* skip commits which have identical trees but only
       in the simple case of one parent */
    if (commit_list.len == 1)
    {
        git_tree *old_tree;

        C(git_commit_tree(&old_tree, commit_list.list[0]));

        if (tree_equal(old_tree, new_tree))
        {
            /* cache this commit's parent so we can use it later */
            dict_add(tf->deleted_commits, commit_id, commit_list.list[0]);
            if (tree != new_tree)
                git_tree_free(new_tree);
            git_tree_free(old_tree);
            commit_list_free(&commit_list);
            return;
        }
        git_tree_free(old_tree);
    }
    else if (commit_list.len > 1)
    {
        unsigned int simplified = 0;
        unsigned int index = 1;
        unsigned int i;

        for(i = 1; i < commit_list.len; i++)
        {
            if (parent_of(tf->repo,
                        git_commit_id(commit_list.list[i]),
                        git_commit_id(commit_list.list[0])))
            {
                simplified ++;
            }
            else
            {
                if (index < i)
                {
                    commit_list.list[index] = commit_list.list[i];
                }
                index++;
            }
        }
        commit_list.len -= simplified;
        /* this is a merge commit that has collapsed to nothing cache this 
           information */
        if (commit_list.len == 1)
        {
            dict_add(tf->deleted_merges, commit_id, commit_list.list[0]);
            if (tree != new_tree)
                git_tree_free(new_tree);
            commit_list_free(&commit_list);
            return;
        }
    }

    message = git_commit_message(commit);
    committer = git_commit_committer(commit);
    author = git_commit_author(commit);

    C(git_commit_create(&new_commit_id,
                tf->repo, NULL,
                author, committer,
                NULL,
                message, new_tree,
                commit_list.len, commit_list.list));

    if (tree != new_tree)
        git_tree_free(new_tree);

    commit_list_free(&commit_list);

    C(git_commit_lookup(&new_commit, tf->repo, &new_commit_id));

    dict_add(tf->revdict, commit_id, new_commit);
    tf->last = new_commit_id;
}


#define CONFIG_KEYLEN 5
void parse_config_file(const char *cfgfile)
{
    FILE *f;
    unsigned int lineno;
    char *base = strdup("");

    f = fopen(cfgfile, "r");
    if (!f)
        die("cannot open %s\n", cfgfile);

    lineno = 0;
    while(!feof(f))
    {
        char *e = local_fgets(f);
        if (!e)
            break;

        lineno ++;

#define VALUE(buf) (buf+CONFIG_KEYLEN+1)

        if (e[0] == '#')
        {
            free(e);
            continue;
        }

        if (!strncmp(e, "REPO: ", CONFIG_KEYLEN))
        {
            if (git_repo_name)
                die("can only specify one repository in %s at %d\n",
                        cfgfile, lineno);
            git_repo_name = strdup(VALUE(e));
        }
        if (!strncmp(e, "TPFX: ", CONFIG_KEYLEN))
        {
            if (git_tag_prefix)
                die("can only specify one tag prefix in %s at %d\n",
                        cfgfile, lineno);
            git_tag_prefix = strdup(VALUE(e));
        }
        if (!strncmp(e, "REVN: ", CONFIG_KEYLEN))
        {
            if (rev_type)
                die("can only specify one revision in %s at %d\n",
                        cfgfile, lineno);
            rev_type = strdup(VALUE(e));
            rev_string = strchr(rev_type, ' ');
            if (!rev_string)
                die("can't find revision %s at %d\n", cfgfile, lineno);
            *rev_string = 0;
            rev_string ++;
        }
        if (!strncmp(e, "BASE: ", CONFIG_KEYLEN))
        {
            free(base);
            base = strdup(VALUE(e));
        }
        if (!strncmp(e, "FILT: ", CONFIG_KEYLEN))
        {
            char *name = VALUE(e);
            char *file = strchr(name, ' ');
            if (!file)
                die("invalid syntax for filter in %s at %d\n",
                        cfgfile, lineno);
            *file = 0;
            file ++;

            file = local_sprintf("%s%s", base, file);

            tf_list_new(strdup(name), file);
        }

        free(e);
    }

    if (rev_string == 0)
        die("no REVN: line found in %s\n", cfgfile);
    if (git_tag_prefix == 0)
        die("no TPFX: line found in %s\n", cfgfile);
    if (git_repo_name == 0)
        die("no REPO: line found in %s\n", cfgfile);
    if (tf_len == 0)
        die("no fiter specified in %s\n", cfgfile);

    free(base);

    fclose(f);
}

static void revwalk_init(git_revwalk *walker, const git_oid *last_commit_id)
{
    git_revwalk_sorting(walker, GIT_SORT_REVERSE|GIT_SORT_TOPOLOGICAL);

    if (!strcmp(rev_type, "ref"))
    {
        C(git_revwalk_push_ref(walker, rev_string));
        if (continue_run)
            C(git_revwalk_hide(walker, last_commit_id));
    }
    else if (!strcmp(rev_type, "range"))
    {
        if (continue_run)
            die("cannot continue from a range");

        C(git_revwalk_push_range(walker, rev_string));
    }
    else
    {
        die("invalid revision type %s in REVN", rev_type);
    }
}

void display_progress(char *s, unsigned int count,
        unsigned int total, time_t start, int force)
{
    time_t now;
    unsigned int percent;
    unsigned int time_taken;
    unsigned int time_remaining;
    static time_t last = 0;

    now = time(0);

    if (!force && now - last == 0)
        return;

    percent = count * 100 / total;
    time_taken = now - start;
    time_remaining = time_taken * (total - count)/ count;

    fprintf(stderr, "%s: %d%% (%d/%d) time %ds(%ds)    \r",
            s, percent, count, total, time_taken, time_remaining);
#if 0
    fflush(stdout);
#endif

    last = now;
}


int main(int argc, char *argv[])
{
    git_repository *repo;
    git_revwalk *walker;
    git_oid commit_oid;
    unsigned int count;
    unsigned int commit_count;
    unsigned int i;
    git_oid last_commit_id;
    char *last_commit_path = 0;
    time_t start;

#if 1
    for (i=0; i<LIST_LEN; i++)
    {
        C(git_oid_fromstr(&search_oid[i], oid_list[i]));
    }
#endif

    if (argc < 2)
    {
        printf("please specify the location of a filter configuration\n");
        printf("%s <filter config> [continue]\n", argv[0]);
        exit(1);
    }

    parse_config_file(argv[1]);

    C(git_repository_open(&repo, git_repo_name));
    if (!git_repository_is_bare(repo))
    {
        git_repo_suffix = ".git/";
    }

    last_commit_path = savefile("last_commit", "");

    if (argc > 2)
    {
        if(!strcmp(argv[2], "continue"))
        {    
            printf("Continuing from previous runs.\n");
            continue_run = 1;
            read_last_commit(&last_commit_id, last_commit_path);
        }
        else
        {
            die("second argument '%s' given. Did you mean 'continue'?",
                    argv[2]);
        }

    }

    for (i = 0; i < tf_len; i++)
        tree_filter_init(&tf_list[i], repo);

    C(git_revwalk_new(&walker, repo));

    revwalk_init(walker, &last_commit_id);

    commit_count = 0;
    while (!git_revwalk_next(&commit_oid, walker))
        commit_count ++;

    if (commit_count == 0)
    {
        printf("No new commits.\n");
        exit(0);
    }

    revwalk_init(walker, &last_commit_id);

    count = 0;
    start = time(0);

    while (!git_revwalk_next(&commit_oid, walker)) {
        git_tree *tree;
        git_commit *commit;

        C(git_commit_lookup(&commit, repo, &commit_oid));
        C(git_commit_tree(&tree, commit));

        for (i = 0; i < tf_len; i++)
            create_commit(&tf_list[i], tree, commit, &commit_oid);

        count ++;
        display_progress("Processing commits", count, commit_count, start, 0);

        git_commit_free(commit);
        git_tree_free(tree);
    }

    display_progress("Processing commits", count, commit_count, start, 1);

    fprintf(stderr, "\n");

    start = time(0) - start;

    fprintf(stderr, "Processed %d new commit%s in %lds.\n",
            count, count > 1 ? "s" : "", start);

    for (i = 0; i < tf_len; i++)
    {
        char oids[GIT_OID_HEXSZ+1];
        char *tag;
        tree_filter_t *tf = &tf_list[i];

        if (!git_oid_iszero(&tf->last))
        {
            tag = local_sprintf("refs/heads/%s%s", git_tag_prefix, tf->name);
            C(git_reference_create(0, tf->repo, tag, &tf->last, 1));
            fprintf(stderr, "branch %s%s is now at %s\n",
                    git_tag_prefix, tf->name,
                    git_oid_tostr(oids, GIT_OID_HEXSZ+1, &tf->last));
            free(tag);
        } else {
            fprintf(stderr, "%s%s unchanged\n", git_tag_prefix, tf->name);
        }

        rev_info_dump(tf);

        tree_filter_fini(&tf[i]);
    }

    save_last_commit(&commit_oid, last_commit_path);
    free(last_commit_path);

    git_revwalk_free(walker);
    git_repository_free(repo);

#if MALLOC_STATS
    malloc_stats();
#endif

    return 0;
}
