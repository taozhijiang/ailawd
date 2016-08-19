-- MySQL dump 10.16  Distrib 10.1.16-MariaDB, for Linux (x86_64)
--
-- Host: 192.168.1.233    Database: v5_law
-- ------------------------------------------------------
-- Server version	10.1.14-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `v5_law`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `v5_law` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci */;

USE `v5_law`;

--
-- Table structure for table `v5_law_meta`
--

DROP TABLE IF EXISTS `v5_law_meta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `v5_law_meta` (
  `uuid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `WS_ID` char(36) COLLATE utf8_unicode_ci NOT NULL COMMENT '文书ID',
  `DQ` varchar(256) COLLATE utf8_unicode_ci NOT NULL COMMENT '地区',
  `YG` varchar(256) COLLATE utf8_unicode_ci NOT NULL COMMENT '原告',
  `YGDLR` varchar(256) COLLATE utf8_unicode_ci NOT NULL COMMENT '原告代理人',
  `BG` varchar(256) COLLATE utf8_unicode_ci NOT NULL COMMENT '被告',
  `BGDLR` varchar(256) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '被告代理人',
  `SPZ` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '审判长',
  `SPY` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '审判员',
  `PSY` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '陪审员',
  `SJY` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '书记员',
  `CPRQ` datetime DEFAULT NULL COMMENT '裁判日期',
  `AJMC` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '案件名称',
  `SPCX` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '审判程序',
  `AH` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '案号',
  `FYMC` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '法院名称',
  PRIMARY KEY (`uuid`),
  UNIQUE KEY `wenshu_id_unique` (`WS_ID`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `v5_law_meta`
--

LOCK TABLES `v5_law_meta` WRITE;
/*!40000 ALTER TABLE `v5_law_meta` DISABLE KEYS */;
INSERT INTO `v5_law_meta` VALUES (1,'9a17a74d-0985-4c97-9249-fb3326af189f','河南省南阳市卧龙区人民法院','李会君','刘超，河南博音律师事务所律师','关晓东',NULL,NULL,'李青',NULL,'申清营','2013-01-01 00:00:00','原告李会君与被告关晓东民间借贷纠纷一案一审民事判决书','一审','（2013）宛龙梅民初字第305号','南阳市卧龙区人民法院'),(2,'4a27cc6f-d4b5-41a9-b71d-46187efd2cbd','湖北省武汉市江岸区人民法院','朱浩','','黄绍彬，武汉佳和船务公司法人。',NULL,'万世红',NULL,'乔佳琳，黎显爱','王珏','2012-05-01 00:00:00','朱浩与黄绍彬民间借贷纠纷一审民事判决书','一审','（2013）鄂江岸民初字第01892号','武汉市江岸区人民法院'),(3,'1ad0eb97-c3b6-4d1e-be5c-15eed1a079aa','河南省南阳市卧龙区人民法院','苏天晔','李新建，河南南都律师事务所律师。','李清印',NULL,NULL,'李青',NULL,'申清营','2015-01-01 00:00:00','原告苏天晔与被告李清印民间借贷纠纷一案一审民事判决书','一审','（2013）宛龙梅民初字第153号','南阳市卧龙区人民法院'),(4,'b4351e2c-f990-4f40-b788-82a492cc8737','湖北省武汉市江岸区人民法院','黄永华','魏莱（一般授权代理），湖北楚风德浩律师事务所律师。','储江',NULL,'夏煜','万世红','乔佳琳','王珏','2015-01-01 00:00:00','黄永华与储江、陆丹民间借贷纠纷一审民事判决书','一审','（2012）鄂江岸民初字第03501号','武汉市江岸区人民法院'),(5,'fb7f3106-4174-478a-8d17-26c41f16ae7c','广东省广州市南沙区人民法院','朱文贺','','叶庆辉',NULL,'赵岚','梁俊聪','陈金好','李帅','2016-01-01 00:00:00','朱文贺与叶庆辉民间借贷纠纷44一审民事判决书','一审','（2013）穗南法南民初字第44号','广州市南沙区人民法院');
/*!40000 ALTER TABLE `v5_law_meta` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `v5_law_text`
--

DROP TABLE IF EXISTS `v5_law_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `v5_law_text` (
  `uuid` bigint(20) NOT NULL AUTO_INCREMENT,
  `WS_ID` char(36) COLLATE utf8_unicode_ci NOT NULL COMMENT '文书ID',
  `full_text` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `AJCX` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '案件程序',
  `YGSC` varchar(255) COLLATE utf8_unicode_ci NOT NULL COMMENT '原告诉称',
  `YGQQPL` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '原告请求判令',
  `YGJZ` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '原告举证',
  `BGBC` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '被告辩称',
  `FYCXZJ` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '法院采信证据',
  `FYCMSS` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '法院查明事实',
  `FYSLYJ` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '法院审理意见',
  `FYPJ` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '法院判决',
  `SSZY` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '上诉指引',
  PRIMARY KEY (`uuid`),
  KEY `wenshu_id` (`WS_ID`),
  CONSTRAINT `wenshu_id` FOREIGN KEY (`WS_ID`) REFERENCES `v5_law_meta` (`WS_ID`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `v5_law_text`
--

LOCK TABLES `v5_law_text` WRITE;
/*!40000 ALTER TABLE `v5_law_text` DISABLE KEYS */;
INSERT INTO `v5_law_text` VALUES (1,'9a17a74d-0985-4c97-9249-fb3326af189f','',NULL,'原告诉称，被告关晓东于2011年11月23日向原告借款320000元，约定月息2分，借期4个月，并向原告出具借条一份。后原告要求被告归还借款，但被告一拖再拖，无奈，故诉至法院，','被告立即支付原告320000元借款，并按约定支付利息承担本案诉讼费用。',NULL,'被告关晓东经本院送达起诉书副本及开庭传票，其在指定的期限内即未提出答辩也未参加诉讼。',NULL,'经审理查明，原、被告系朋友关系，被告于2011年11月23日向原告借款320000元并出具了借条一份。内容为“借条：今借李会君人民币叁拾贰万元整，月息2分，借期肆个月。借款人：关晓东，2011.11.23日”借款到期后原告多次向被告催要无果双方引起纠纷，诉至法院。\n以上事实，有原告的陈述，被告书写的借据及相关书证材料，并经当庭出示、宣读、记录在案，足以认定。','本院认为，被告向原告借款320000元，并向原告出具有借据，约定利息，双方的民间借贷合同依法成立。原告向被告催要借款时，被告理应及时归还借款并支付利息。拒不还款属违约行为，故原告李会君要求被告关晓东偿还320000元借款及利息的请求理由正当。本院依法向被告送达了起诉状副本，在规定的时间内被告未对其提出答辩，对其抗辩理由自动放弃，应视为对该借款的认可。本案事实清楚，责任明确。依据《中华人民共和国民法通则》第八十四条、《中华人民共和国合同法》第一百九十六条、《中华人民共和国民事诉讼法》第一百四十四条之规定，判','自本判决生效后十日内，被告关晓东向原告李会君偿还借款320000元，并自2011年11月23日起按月息2分给付利息至欠款付清之日止。\n诉讼费6000元由被告关晓东负担。','如不服本判决，可在判决书送达之日起十五日内，向本院递交上诉状，并按对方当事人人数提出副本，上诉于河南省南阳市中级人民法院。'),(2,'4a27cc6f-d4b5-41a9-b71d-46187efd2cbd','','原告朱浩诉被告黄绍彬民间借贷纠纷一案，本院于2013年5月22日立案受理。因被告黄绍彬下落不明，本院于2013年8月10日通过公告送达方式向被告黄绍彬送达民事起诉状副本、应诉通知书、举证通知书、开庭传票等诉讼文书。经本院院长批准，本案转为普通程序审理，并依法组成由审判员万世红担任审判长，和人民陪审员乔佳琳、黎显爱参加的合议庭，于2013年11月11日公开开庭进行了审理。原告朱浩到庭参加诉讼。被告黄绍彬经本院合法传唤无正当理由拒不到庭，本院依法缺席进行了审理。本案现已审理终结。','原告朱浩诉称：2011年11月29日和2011年12月6日，被告黄绍彬分两次向我借款共计人民币300，000元，并出具《借条》2份。但还款期限届满后，被告黄绍彬一直未归还借款及利息，其行为严重损害了我的合法权益。','1、被告黄绍彬偿还欠款人民币300，000元及利息（从起诉之日起按中国人民银行同期同类贷款基准利率计算）；2、被告黄绍彬承担本案全部诉讼费用。','原告朱浩为支持其诉讼主张，向本院提交如下证据：\n证据一、《借条》二份，证明被告黄绍彬向其借款共计300，000元的事实。\n证据二、《中国建设银行转款凭条》一份，证明其于2011年12月6日向被告黄绍彬的账上转款94，000元的事实。','被告黄绍彬在法定期限内未到庭参加诉讼，亦未提交答辩意见。','经庭审认证，原告朱浩提交的证据与本案具有关联性，能够证明案件的事实，本院均予以采信。','经审理查明：原告朱浩与被告黄绍彬系朋友关系。2011年11月29日，被告黄绍彬向原告朱浩出具《借条》一份。该《借条》载明：“借到朱浩人民币壹拾万圆正（10，000、），两个月归还。黄绍彬2011年11月29日。”2011年12月6日，被告黄绍彬再次向原告朱浩出具《借条》一份。该《借条》载明：“今借到朱浩人民币式拾万圆正（20，000、），两个月。黄绍彬2011年12月6日。”同日，原告朱浩通过其账户（账号为：62×××74）向被告黄绍彬的账户上（账号为：43×××46）转款94，000元。审理中，原告朱浩','本院认为：原告朱浩与被告黄绍彬之间的借款行为是自然人之间的个人借款，属于民间借贷，其借贷行为是双方当事人的真实意思表示，应受法律保护。原告朱浩依双方的约定，已履行了借款义务。但被告黄绍彬未依约履行偿还借款的义务，其行为属于违约行为，理应承担相应的民事责任。因双方在《借条》中均未约定利息，且原告朱浩自认其在借款给被告黄绍彬时已收到利息共计9，000元，故该款项应冲抵本金。现原告朱浩要求被告黄绍彬偿还借款本金300，000元的诉讼请求，本院不予全部支持。被告黄绍彬应按实际借款金额291，000元向原告朱浩偿还','一、被告黄绍彬向原告朱浩偿还借款本金人民币291，000元；\n二、被告黄绍彬向原告朱浩赔偿利息损失（以人民币291，000元为本金，按中国人民银行同期同类贷款基准利率计算利息，从2013年5月22日起至本判决确认给付之日时止）；\n三、驳回原告朱浩的其他诉讼请求。\n上列应付款项，于本判决生效之日起十日内付清。逾期支付，按《中华人民共和国民事诉讼法》第二百五十三条的规定办理。\n案件受理费人民币5，800元、公告送达费人民币560元，共计6，360元由被告黄绍彬负担。','如不服本判决，可在判决书送达之日起十五日内，向本院递交上诉状，并按对方当事人的人数提出副本，上诉于湖北省武汉市中级人民法院。'),(3,'1ad0eb97-c3b6-4d1e-be5c-15eed1a079aa','','原告苏天晔与被告李清印民间借贷纠纷一案，本院受理后，由审判员李青适用简易程序，于2013年7月26日公开开庭对本案进行了审理。原告苏天晔的委托代理人李新建到庭参加了诉讼，被告李清印经本院公告送达开庭传票其无正当理由未到庭。本案现已审理终结。','原告诉称，被告李清印于2011年7月22日至2012年9月分7次向原告借款共160000元，并向原告书写借条七份。后','被告归还借款，但被告一拖再拖，并躲避不见，拖延至今不还，无奈诉至法院，请求依法判令被告立即支付原告借款160000元，并按约定支付利息，承担本案诉讼费用。',NULL,'被告李清印经本院公告送达起诉书副本及开庭传票，其在指定的期限内即未提出答辩也未参加诉讼。',NULL,'经审理查明，原、被告原系朋友关系，被告分别于2011年7月22日、2012年3月9日、2012年3月17日、2012年3月19日、2012年3月21日、2012年5月12日、2012年9月12日共向原告借款160000元，并书写借据七份。其中2012年3月9日、2012年3月17日、2012年3月19日、2012年3月21日四笔借款共计60000元约定月息千分之二十，后原告多次向被告催要无果双方引起纠纷，诉至法院。\n以上事实，有原告的陈述，被告书写的借据及相关书证材料，并经当庭出示、宣读、记录在案，足以认','本院认为，被告向原告借款160000元，且向原告出具有借据，并部分约定了利息，双方的民间借贷合同依法成立。原告向被告催要借款时，被告理应及时归还借款并支付约定利息。但被告未予履行，属违约行为，原告起诉的请求事实清楚，理应正当，本院依法予以支持。被告应当归还原告借款160000元，其中2012年3月9日、2012年3月17日、2012年3月19日、2012年3月21日60000元自2012年3月21日起按约定的月利率千分之二十计付利息。2011年7月22日60000元，2012年5月12日30000元，20','自本判决生效后十日内，被告李清印支付原告苏天晔借款160000元，其中100000元自2013年4月4日起按中国人民银行同期贷款利率支付利息至欠款付清之日止；其中60000元自2012年3月21日按月利率千分之二十计息至欠款付清之日止。\n诉讼费3500元减半收取，保全费1320元由被告李清印负担。','如不服本判决，可在判决书送达之日起十五日内，向本院递交上诉状，并按对方当事人人数提出副本，上诉于河南省南阳市中级人民法院。'),(5,'b4351e2c-f990-4f40-b788-82a492cc8737','','原告黄永华诉被告储江、被告陆丹民间借贷纠纷一案，2012年7月2日原告黄永华向本院申请诉前财产保全，要求冻结被告储江、被告陆丹在银行帐户上存款人民币2，000，000元或查封、扣押其等值财产。2012年7月3日，本院作出（2012）鄂江岸保字第00069号民事裁定书并依原告黄永华的申请，依法查封了被告储江所有的坐落在武汉市江岸金源世界中心A座20层1室、2室、4室，B座20层1室、3室房屋五套和被告陆丹所有的坐落在武汉市江岸区同安家园一17栋18层3室房屋一套。本院于2012年7月25日立案受理。审理中，','原告黄永华诉称：2009年4月30日、2009年8月25日、2010年7月23日、9月6日，2011年3月18日、7月29日、9月16日，被告储江先后分别向我借款共计人民币1，780，000元。借款后，我多次向被告储江催要未果。被告储江和被告陆丹系夫妻关系，依据最高人民法院《关于适用﹤中华人民共和国婚姻法﹥若干问题的解释（二）》第二十四条的规定。被告储江个人名义出具的欠条，是被告储江与被告陆丹夫妻关系存续期间的债务，属于夫妻共同债务。应共同偿还。现向法院起诉，','1、被告储江、被告陆丹共同偿还借款人民币1，780，000元；2、被告储江、被告陆丹自原告黄永华起诉之日起按同期银行贷款利率的四倍向原告黄永华支付利息。3、本案诉讼费由被告储江、被告陆丹共同承担。','原告黄永华为支持其诉讼主张，向本院提交如下证据：\n证据一：《借条》一份。证明2010年7月23日被告储江向原告黄永华借款人民币80，000元现金的事实。\n证据二：《借条》一份。证明2010年9月6日被告储江向原告黄永华借款人民币500，000元的事实。\n证据三：《借条》一份。证明2011年3月18日被告储江向原告黄永华借款人民币100，000元的事实。\n证据四：《借条》、《转账回单》各一份。证明2011年7月29日被告储江向原告黄永华借款人民币600，000元的事实；\n证据五：《借条》一份。证明2011年','被告储江辩称：第一、目前我尚欠原告黄永华人民币300，000元债务是事实；第二、2011年7月份的人民币600，000元借款实际是案外人祝丹借出的；第三、我已向原告黄永华偿还借款人民币847，000元、偿还祝丹人民币308，000元的；第四、所有的借款均属于我个人借款，用于武汉智巢科技发展有限公司的经营。所借债务与他人无关。被告陆丹辩称：我对原告黄永华起诉的借款，不应承担民事责任。第一、对于原告黄永华所诉的所有借款我都不清楚；第二、我没有见过原告黄永华本人，根本不认识原告黄永华；第三、我与被告储江已于20',NULL,'本院依职权对案外人祝丹、付凡华进行调查。祝丹证明其向被告储江转款人民币500，000元是受原告黄永华的委托，该债权属于原告黄永华所有。付凡华证明其向被告储江转款人民币190，000元是受原告黄永华的委托，该债权属于原告黄永华所有。',NULL,'一、被告储江、被告陆丹在本判决书生效后十日内共同向原告黄永华偿还借款人民币833，000元。\n[无效数据]\n二、被告储江、被告陆丹在本判决书生效后十日内共同向原告黄永华支付利息（以本金人民币833，000元为基点按中国人民银行同期同类贷款基准利率计算利息；从2012年7月25日起至本判决确认给付之时止）；\n三、驳回原告黄永华的其他诉讼请求。','如不服本判决，可在判决书送达之日起十五日内，向本院递交上诉状，并按对方当事人的人数提出副本，上诉于湖北省武汉市中级人民法院。'),(6,'fb7f3106-4174-478a-8d17-26c41f16ae7c','','原告朱文贺诉被告叶庆辉民间借贷纠纷一案，本院受理后，依法组成合议庭公开开庭进行了审理。原告朱文贺到庭参加诉讼。被告叶庆辉因下落不明，经本院公告送达，未到庭参加诉讼，本院依法缺席审理。本案现已审理终结。','原告诉称，被告在2012年10月19日到原告家中请求帮忙，向原告借款30000元，并亲笔书写《借据》一份，约定借款30000元，月利息600元。被告至今未还款付息，据此，','被告清偿借款30000元及支付利息（自2012年10月19日起，按中国人民银行贷款利率的四倍计至还款之日止）。',NULL,'被告无答辩亦无证据提交。',NULL,'经审理查明，2012年10月19日，被告叶庆辉书写《借据》一份，内容为：“2012年10月19日，叶庆辉到朱文贺家塘坑中街125号2楼，要求朱文贺帮忙困难，因老婆冼巧谊母亲在医院住院需医疗费，现要求朱文贺借到叁万元正人民币现金（30000元），月利息600元，按每月19日付利息。……借款人：叶庆辉，日期：2012年10月19日”。\n被告未依约向原告偿还借款本金及利息。','本院认为，合法的借贷关系受法律保护。被告向原告借款并立下《借据》，《借据》是原告与被告之间建立借贷关系的凭证。被告借款至今未还款付息，构成违约，应当承担违约责任，原告要求被告偿还借款及支付利息符合法律规定，本院予以支持。根据《最高人民法院关于人民法院审理借贷案件的若干意见》第6条规定：“民间借贷的利率可以适当高于银行的利率，……，但最高不得超过银行同类贷款利率的四倍（包含利率本数）。超出此限度的，超出部分的利息不予保护”，《借据》中约定借款每月利息600元未违反国家有关限制借款利率的规定，故原告请求按银行','一、在本判决发生法律效力之日起10日内，被告叶庆辉返还原告朱文贺借款本金30000元及支付利息（自2012年10月19日起至还款之日止，按每月利息600元计付）。\n二、驳回原告朱文贺的其他诉讼请求。','如不服本判决，可在判决书送达之日起15日内，向本院递交上诉状，并按对方当事人的人数提出副本，上诉于广东省广州市中级人民法院。');
/*!40000 ALTER TABLE `v5_law_text` ENABLE KEYS */;
UNLOCK TABLES;


--
-- Table structure for table `v5_law_info`
--

DROP TABLE IF EXISTS `v5_law_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `v5_law_info` (
  `uuid` bigint(20) NOT NULL AUTO_INCREMENT,
  `WS_ID` char(36) COLLATE utf8_unicode_ci NOT NULL,
  `DQ_S` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL COMMENT '地区_',
  PRIMARY KEY (`uuid`),
  KEY `wenshu_ID_f` (`WS_ID`),
  CONSTRAINT `wenshu_ID_f` FOREIGN KEY (`WS_ID`) REFERENCES `v5_law_meta` (`WS_ID`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `v5_law_info`
--

LOCK TABLES `v5_law_info` WRITE;
/*!40000 ALTER TABLE `v5_law_info` DISABLE KEYS */;
INSERT INTO `v5_law_info` VALUES (1,'9a17a74d-0985-4c97-9249-fb3326af189f','河南'),(2,'4a27cc6f-d4b5-41a9-b71d-46187efd2cbd','湖北'),(3,'1ad0eb97-c3b6-4d1e-be5c-15eed1a079aa','河南'),(4,'b4351e2c-f990-4f40-b788-82a492cc8737','湖北'),(5,'fb7f3106-4174-478a-8d17-26c41f16ae7c','广东');
/*!40000 ALTER TABLE `v5_law_info` ENABLE KEYS */;
UNLOCK TABLES;


/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2016-08-19  9:22:30
