//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

// missing messages
//0x0BE5, 0x0BE6, 0x0BE7, 0x0BE8, 0x0BEA

#pragma once

static inline constexpr uint16_t DEF_NOTIFY_SPAWNEVENT = 0x0BAA;
static inline constexpr uint16_t DEF_NOTIFY_QUESTCOUNTER = 0x0BE2;

// NetMessages.h

static inline constexpr uint16_t DEF_MSGTYPE_CONFIRM = 0x0F14;
static inline constexpr uint16_t DEF_MSGTYPE_REJECT = 0x0F15;

static inline constexpr uint32_t MSGID_REQUEST_INITPLAYER = 0x05040205;
static inline constexpr uint32_t MSGID_RESPONSE_INITPLAYER = 0x05040206;

static inline constexpr uint32_t MSGID_REQUEST_INITDATA = 0x05080404;
static inline constexpr uint32_t MSGID_RESPONSE_INITDATA = 0x05080405;

static inline constexpr uint32_t MSGID_COMMAND_MOTION = 0x0FA314D5;
static inline constexpr uint32_t MSGID_RESPONSE_MOTION = 0x0FA314D6;
static inline constexpr uint32_t MSGID_EVENT_MOTION = 0x0FA314D7;
static inline constexpr uint32_t MSGID_EVENT_LOG = 0x0FA314D8;
static inline constexpr uint32_t MSGID_EVENT_COMMON = 0x0FA314DB;
static inline constexpr uint32_t MSGID_COMMAND_COMMON = 0x0FA314DC;
static inline constexpr uint32_t MSGID_SCREEN_SETTINGS = 0x0FA314DD;

static inline constexpr uint16_t DEF_COMMONTYPE_ITEMDROP = 0x0A01;
static inline constexpr uint16_t DEF_COMMONTYPE_EQUIPITEM = 0x0A02;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_LISTCONTENTS = 0x0A03;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_PURCHASEITEM = 0x0A04;
static inline constexpr uint16_t DEF_COMMONTYPE_GIVEITEMTOCHAR = 0x0A05;
static inline constexpr uint16_t DEF_COMMONTYPE_JOINGUILDAPPROVE = 0x0A06;
static inline constexpr uint16_t DEF_COMMONTYPE_JOINGUILDREJECT = 0x0A07;
static inline constexpr uint16_t DEF_COMMONTYPE_DISMISSGUILDAPPROVE = 0x0A08;
static inline constexpr uint16_t DEF_COMMONTYPE_DISMISSGUILDREJECT = 0x0A09;
static inline constexpr uint16_t DEF_COMMONTYPE_RELEASEITEM = 0x0A0A;
static inline constexpr uint16_t DEF_COMMONTYPE_TOGGLECOMBATMODE = 0x0A0B;
static inline constexpr uint16_t DEF_COMMONTYPE_SETITEM = 0x0A0C;
static inline constexpr uint16_t DEF_COMMONTYPE_MAGIC = 0x0A0D;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_STUDYMAGIC = 0x0A0E;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_TRAINSKILL = 0x0A0F;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETREWARDMONEY = 0x0A10;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_USEITEM = 0x0A11;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_USESKILL = 0x0A12;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_SELLITEM = 0x0A13;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_REPAIRITEM = 0x0A14;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_SELLITEMCONFIRM = 0x0A15;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_REPAIRITEMCONFIRM = 0x0A16;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETFISHTHISTIME = 0x0A17;
static inline constexpr uint16_t DEF_COMMONTYPE_TOGGLESAFEATTACKMODE = 0x0A18;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_CREATEPOTION = 0x0A19;
static inline constexpr uint16_t DEF_COMMONTYPE_TALKTONPC = 0x0A1A;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_SETDOWNSKILLINDEX = 0x0A1B;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETOCCUPYFLAG = 0x0A1C;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETHEROMANTLE = 0x0A1D;
// static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETANGELMANTLE = 0x0FC9421E;
static inline constexpr uint16_t DEF_COMMONTYPE_EXCHANGEITEMTOCHAR = 0x0A1E;
static inline constexpr uint16_t DEF_COMMONTYPE_SETEXCHANGEITEM = 0x0A1F;
static inline constexpr uint16_t DEF_COMMONTYPE_CONFIRMEXCHANGEITEM = 0x0A20;
static inline constexpr uint16_t DEF_COMMONTYPE_CANCELEXCHANGEITEM = 0x0A21;
static inline constexpr uint16_t DEF_COMMONTYPE_QUESTACCEPTED = 0x0A22;
static inline constexpr uint16_t DEF_COMMONTYPE_BUILDITEM = 0x0A23;
static inline constexpr uint16_t DEF_COMMONTYPE_GETMAGICABILITY = 0x0A24;
static inline constexpr uint16_t DEF_COMMONTYPE_CLEARGUILDNAME = 0x0A25;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETDARKITEM = 0x0A26;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET = 0x0A25;
static inline constexpr uint16_t DEF_COMMONTYPE_BANGUILD = 0x0A26;
static inline constexpr uint16_t DEF_COMMONTYPE_CRAFTITEM = 0x0A28; // Craft item

static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY = 0x0A30;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_JOINPARTY = 0x0A31;
static inline constexpr uint16_t DEF_COMMONTYPE_RESPONSE_JOINPARTY = 0x0A32;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_ACTIVATESPECABLTY = 0x0A40;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_CANCELQUEST = 0x0A50;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_SELECTCRUSADEDUTY = 0x0A51;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_MAPSTATUS = 0x0A52;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_HELP = 0x0A53;

static inline constexpr uint16_t DEF_COMMONTYPE_SETGUILDTELEPORTLOC = 0x0A54;
static inline constexpr uint16_t DEF_COMMONTYPE_GUILDTELEPORT = 0x0A55;
static inline constexpr uint16_t DEF_COMMONTYPE_SUMMONWARUNIT = 0x0A56;
static inline constexpr uint16_t DEF_COMMONTYPE_SETGUILDCONSTRUCTLOC = 0x0A57;

static inline constexpr uint32_t MSGID_NOTIFY = 0x0FA314D0;

static inline constexpr uint16_t DEF_NOTIFY_ITEMOBTAINED = 0x0B01;
static inline constexpr uint16_t DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION = 0x0B02;
static inline constexpr uint16_t DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION = 0x0B03;
static inline constexpr uint16_t DEF_NOTIFY_WAITFORGUILDOPERATION = 0x0B04;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTCARRYMOREITEM = 0x0B05;
static inline constexpr uint16_t DEF_NOTIFY_ITEMPURCHASED = 0x0B06;
static inline constexpr uint16_t DEF_NOTIFY_HP = 0x0B07;
static inline constexpr uint16_t DEF_NOTIFY_NOTENOUGHGOLD = 0x0B08;
static inline constexpr uint16_t DEF_NOTIFY_KILLED = 0x0B09;
static inline constexpr uint16_t DEF_NOTIFY_EXP = 0x0B0A;
static inline constexpr uint16_t DEF_NOTIFY_GUILDDISBANDED = 0x0B0B;
static inline constexpr uint16_t DEF_NOTIFY_EVENTMSGSTRING = 0x0B0C;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTJOINMOREGUILDSMAN = 0x0B0D;
static inline constexpr uint16_t DEF_NOTIFY_NEWGUILDSMAN = 0x0B0E;
static inline constexpr uint16_t DEF_NOTIFY_DISMISSGUILDSMAN = 0x0B0F;
static inline constexpr uint16_t DEF_NOTIFY_MAGICSTUDYSUCCESS = 0x0B10;
static inline constexpr uint16_t DEF_NOTIFY_MAGICSTUDYFAIL = 0x0B11;
static inline constexpr uint16_t DEF_NOTIFY_SKILLTRAINSUCCESS = 0x0B12;
static inline constexpr uint16_t DEF_NOTIFY_SKILLTRAINFAIL = 0x0B13;
static inline constexpr uint16_t DEF_NOTIFY_MP = 0x0B14;
static inline constexpr uint16_t DEF_NOTIFY_SP = 0x0B15;
static inline constexpr uint16_t DEF_NOTIFY_LEVELUP = 0x0B16;
static inline constexpr uint16_t DEF_NOTIFY_ITEMLIFESPANEND = 0x0B17;
static inline constexpr uint16_t DEF_NOTIFY_LIMITEDLEVEL = 0x0B18;
static inline constexpr uint16_t DEF_NOTIFY_ITEMTOBANK = 0x0B19;
static inline constexpr uint16_t DEF_NOTIFY_PKPENALTY = 0x0B1A;
static inline constexpr uint16_t DEF_NOTIFY_PKCAPTURED = 0x0B1B;
static inline constexpr uint16_t DEF_NOTIFY_ENEMYKILLREWARD = 0x0B1C;
static inline constexpr uint16_t DEF_NOTIFY_GIVEITEMFIN_ERASEITEM = 0x0B1D;
static inline constexpr uint16_t DEF_NOTIFY_DROPITEMFIN_ERASEITEM = 0x0B1F;
static inline constexpr uint16_t DEF_NOTIFY_ITEMDEPLETED_ERASEITEM = 0x0B20;
static inline constexpr uint16_t DEF_NOTIFY_NEWDYNAMICOBJECT = 0x0B21;
static inline constexpr uint16_t DEF_NOTIFY_DELDYNAMICOBJECT = 0x0B22;
static inline constexpr uint16_t DEF_NOTIFY_SKILL = 0x0B23;
static inline constexpr uint16_t DEF_NOTIFY_SERVERCHANGE = 0x0B24;
static inline constexpr uint16_t DEF_NOTIFY_SETITEMCOUNT = 0x0B25;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTITEMTOBANK = 0x0B26;
static inline constexpr uint16_t DEF_NOTIFY_MAGICEFFECTON = 0x0B27;
static inline constexpr uint16_t DEF_NOTIFY_MAGICEFFECTOFF = 0x0B28;
static inline constexpr uint16_t DEF_NOTIFY_TOTALUSERS = 0x0B29;
static inline constexpr uint16_t DEF_NOTIFY_SKILLUSINGEND = 0x0B2A;
static inline constexpr uint16_t DEF_NOTIFY_SHOWMAP = 0x0B2B;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTSELLITEM = 0x0B2C;
static inline constexpr uint16_t DEF_NOTIFY_SELLITEMPRICE = 0x0B2D;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTREPAIRITEM = 0x0B2E;
static inline constexpr uint16_t DEF_NOTIFY_REPAIRITEMPRICE = 0x0B2F;
static inline constexpr uint16_t DEF_NOTIFY_ITEMREPAIRED = 0x0B30;
static inline constexpr uint16_t DEF_NOTIFY_ITEMSOLD = 0x0B31;
static inline constexpr uint16_t DEF_NOTIFY_CHARISMA = 0x0B32;
static inline constexpr uint16_t DEF_NOTIFY_PLAYERONGAME = 0x0B33;
static inline constexpr uint16_t DEF_NOTIFY_PLAYERNOTONGAME = 0x0B34;
static inline constexpr uint16_t DEF_NOTIFY_WHISPERMODEON = 0x0B35;
static inline constexpr uint16_t DEF_NOTIFY_WHISPERMODEOFF = 0x0B36;
static inline constexpr uint16_t DEF_NOTIFY_PLAYERPROFILE = 0x0B37;
static inline constexpr uint16_t DEF_NOTIFY_TRAVELERLIMITEDLEVEL = 0x0B38;
static inline constexpr uint16_t DEF_NOTIFY_HUNGER = 0x0B39;

static inline constexpr uint16_t DEF_NOTIFY_TOBERECALLED = 0x0B40;
static inline constexpr uint16_t DEF_NOTIFY_TIMECHANGE = 0x0B41;
static inline constexpr uint16_t DEF_NOTIFY_PLAYERSHUTUP = 0x0B42;
static inline constexpr uint16_t DEF_NOTIFY_ADMINUSERLEVELLOW = 0x0B43;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTRATING = 0x0B44;
static inline constexpr uint16_t DEF_NOTIFY_RATINGPLAYER = 0x0B45;
static inline constexpr uint16_t DEF_NOTIFY_NOTICEMSG = 0x0B46;
static inline constexpr uint16_t DEF_NOTIFY_EVENTFISHMODE = 0x0B47;
static inline constexpr uint16_t DEF_NOTIFY_FISHCHANCE = 0x0B48;
static inline constexpr uint16_t DEF_NOTIFY_DEBUGMSG = 0x0B49;
static inline constexpr uint16_t DEF_NOTIFY_FISHSUCCESS = 0x0B4A;
static inline constexpr uint16_t DEF_NOTIFY_FISHFAIL = 0x0B4B;
static inline constexpr uint16_t DEF_NOTIFY_FISHCANCELED = 0x0B4C;
static inline constexpr uint16_t DEF_NOTIFY_WHETHERCHANGE = 0x0B4D;
static inline constexpr uint16_t DEF_NOTIFY_SERVERSHUTDOWN = 0x0B4E;
static inline constexpr uint16_t DEF_NOTIFY_REWARDGOLD = 0x0B4F;
static inline constexpr uint16_t DEF_NOTIFY_IPACCOUNTINFO = 0x0B50;
static inline constexpr uint16_t DEF_NOTIFY_SAFEATTACKMODE = 0x0B51;
static inline constexpr uint16_t DEF_NOTIFY_SUPERATTACKLEFT = 0x0B52;
static inline constexpr uint16_t DEF_NOTIFY_NOMATCHINGPOTION = 0x0B53;
static inline constexpr uint16_t DEF_NOTIFY_LOWPOTIONSKILL = 0x0B54;
static inline constexpr uint16_t DEF_NOTIFY_POTIONFAIL = 0x0B55;
static inline constexpr uint16_t DEF_NOTIFY_POTIONSUCCESS = 0x0B56;
static inline constexpr uint16_t DEF_NOTIFY_NPCTALK = 0x0B57;
static inline constexpr uint16_t DEF_NOTIFY_ADMINIFO = 0x0B58;
static inline constexpr uint16_t DEF_NOTIFY_DOWNSKILLINDEXSET = 0x0B59;
static inline constexpr uint16_t DEF_NOTIFY_ENEMYKILLS = 0x0B5A;
static inline constexpr uint16_t DEF_NOTIFY_ITEMPOSLIST = 0x0B5B;
static inline constexpr uint16_t DEF_NOTIFY_ITEMRELEASED = 0x0B5C;
static inline constexpr uint16_t DEF_NOTIFY_NOTFLAGSPOT = 0x0B5D;
static inline constexpr uint16_t DEF_NOTIFY_OPENEXCHANGEWINDOW = 0x0B5E;
static inline constexpr uint16_t DEF_NOTIFY_SETEXCHANGEITEM = 0x0B5F;
static inline constexpr uint16_t DEF_NOTIFY_CANCELEXCHANGEITEM = 0x0B60;
static inline constexpr uint16_t DEF_NOTIFY_EXCHANGEITEMCOMPLETE = 0x0B61;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTGIVEITEM = 0x0B62;
static inline constexpr uint16_t DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED = 0x0B63;
static inline constexpr uint16_t DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED = 0x0B64;
static inline constexpr uint16_t DEF_NOTIFY_ITEMCOLORCHANGE = 0x0B65;
static inline constexpr uint16_t DEF_NOTIFY_QUESTCONTENTS = 0x0B66;
static inline constexpr uint16_t DEF_NOTIFY_QUESTABORTED = 0x0B67;
static inline constexpr uint16_t DEF_NOTIFY_QUESTCOMPLETED = 0x0B68;
static inline constexpr uint16_t DEF_NOTIFY_QUESTREWARD = 0x0B69;

static inline constexpr uint16_t DEF_NOTIFY_BUILDITEMSUCCESS = 0x0B70;
static inline constexpr uint16_t DEF_NOTIFY_BUILDITEMFAIL = 0x0B71;
static inline constexpr uint16_t DEF_NOTIFY_OBSERVERMODE = 0x0B72;
static inline constexpr uint16_t DEF_NOTIFY_GLOBALATTACKMODE = 0x0B73;
static inline constexpr uint16_t DEF_NOTIFY_DAMAGEMOVE = 0x0B74;
static inline constexpr uint16_t DEF_NOTIFY_FORCEDISCONN = 0x0B75;
static inline constexpr uint16_t DEF_NOTIFY_FIGHTZONERESERVE = 0x0B76;
static inline constexpr uint16_t DEF_NOTIFY_NOGUILDMASTERLEVEL = 0x0B77;
static inline constexpr uint16_t DEF_NOTIFY_SUCCESSBANGUILDMAN = 0x0B78;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTBANGUILDMAN = 0x0B79;

static inline constexpr uint16_t DEF_NOTIFY_RESPONSE_CREATENEWPARTY = 0x0B80;
static inline constexpr uint16_t DEF_NOTIFY_QUERY_JOINPARTY = 0x0B81;

// static inline constexpr uint16_t DEF_NOTIFY_SUCCESSBANGUILDMAN = 0x0B82;


static inline constexpr uint16_t DEF_NOTIFY_ENERGYSPHERECREATED = 0x0B90;
static inline constexpr uint16_t DEF_NOTIFY_ENERGYSPHEREGOALIN = 0x0B91;
static inline constexpr uint16_t DEF_NOTIFY_SPECIALABILITYENABLED = 0x0B92;
static inline constexpr uint16_t DEF_NOTIFY_SPECIALABILITYSTATUS = 0x0B93;
static inline constexpr uint16_t DEF_NOTIFY_CRUSADE = 0x0B94;
static inline constexpr uint16_t DEF_NOTIFY_LOCKEDMAP = 0x0B95;
static inline constexpr uint16_t DEF_NOTIFY_DUTYSELECTED = 0x0B96;
static inline constexpr uint16_t DEF_NOTIFY_MAPSTATUSNEXT = 0x0B97;
static inline constexpr uint16_t DEF_NOTIFY_MAPSTATUSLAST = 0x0B98;
static inline constexpr uint16_t DEF_NOTIFY_HELP = 0x0B99;
static inline constexpr uint16_t DEF_NOTIFY_HELPFAILED = 0x0B9A;
static inline constexpr uint16_t DEF_NOTIFY_METEORSTRIKECOMING = 0x0B9B;
static inline constexpr uint16_t DEF_NOTIFY_METEORSTRIKEHIT = 0x0B9C;
static inline constexpr uint16_t DEF_NOTIFY_GRANDMAGICRESULT = 0x0B9D;
static inline constexpr uint16_t DEF_NOTIFY_NOMORECRUSADESTRUCTURE = 0x0B9E;
static inline constexpr uint16_t DEF_NOTIFY_CONSTRUCTIONPOINT = 0x0B9F;

static inline constexpr uint16_t DEF_NOTIFY_TCLOC = 0x0BA0;
static inline constexpr uint16_t DEF_NOTIFY_CANNOTCONSTRUCT = 0x0BA1;
static inline constexpr uint16_t DEF_NOTIFY_PARTY = 0x0BA2;
static inline constexpr uint16_t DEF_NOTIFY_ITEMATTRIBUTECHANGE = 0x0BA3;
static inline constexpr uint16_t DEF_NOTIFY_GIZONITEMUPGRADELEFT = 0x0BA4;
static inline constexpr uint16_t DEF_NOTIFY_GIZONITEMCHANGE = 0x0BA5;
static inline constexpr uint16_t DEF_NOTIFY_REQGUILDNAMEANSWER = 0x0BA6;
static inline constexpr uint16_t DEF_NOTIFY_REQPLAYERNAMEANSWER = 0x0BFE;
static inline constexpr uint16_t DEF_NOTIFY_FORCERECALLTIME = 0x0BA7;
static inline constexpr uint16_t DEF_NOTIFY_ITEMUPGRADEFAIL = 0x0BA8;
static inline constexpr uint16_t DEF_NOTIFY_RESPONSE_HUNTMODE = 0x0BA9;

static inline constexpr uint16_t DEF_NOTIFY_MONSTEREVENT_POSITION = 0x0BAA;
static inline constexpr uint16_t DEF_NOTIFY_NOMOREAGRICULTURE = 0x0BB0;
static inline constexpr uint16_t DEF_NOTIFY_AGRICULTURESKILLLIMIT = 0x0BB1;
static inline constexpr uint16_t DEF_NOTIFY_AGRICULTURENOAREA = 0x0BB2;
static inline constexpr uint16_t DEF_NOTIFY_SETTING_SUCCESS = 0x0BB3;
static inline constexpr uint16_t DEF_NOTIFY_SETTING_FAILED = 0x0BB4;
static inline constexpr uint16_t DEF_NOTIFY_STATECHANGE_SUCCESS = 0x0BB5;
static inline constexpr uint16_t DEF_NOTIFY_STATECHANGE_FAILED = 0x0BB6;

static inline constexpr uint16_t DEF_NOTIFY_SLATE_CREATESUCCESS = 0x0BC1;
static inline constexpr uint16_t DEF_NOTIFY_SLATE_CREATEFAIL = 0x0BC2;

static inline constexpr uint16_t DEF_NOTIFY_NORECALL = 0x0BD1; // "You cannot recall on this = map"
static inline constexpr uint16_t DEF_NOTIFY_APOCGATESTARTMSG = 0x0BD2; // "The portal to the appoclypse gate is = open."
static inline constexpr uint16_t DEF_NOTIFY_APOCGATEENDMSG = 0x0BD3;
static inline constexpr uint16_t DEF_NOTIFY_APOCGATEOPEN = 0x0BD4;
static inline constexpr uint16_t DEF_NOTIFY_APOCGATECLOSE = 0x0BD5;
static inline constexpr uint16_t DEF_NOTIFY_ABADDONKILLED = 0x0BD6; // "Abaddon destroyed by = (playername)"
static inline constexpr uint16_t DEF_NOTIFY_APOCFORCERECALLPLAYERS = 0x0BD7; // "You are forced to recall, because the apoclypse has = started"
static inline constexpr uint16_t DEF_NOTIFY_SLATE_INVINCIBLE = 0x0BD8;
static inline constexpr uint16_t DEF_NOTIFY_SLATE_MANA = 0x0BD9;

static inline constexpr uint16_t DEF_NOTIFY_SLATE_EXP = 0x0BE0;
static inline constexpr uint16_t DEF_NOTIFY_SLATE_STATUS = 0x0BE1;
static inline constexpr uint16_t DEF_NOTIFY_MONSTERCOUNT = 0x0BE3;
static inline constexpr uint16_t DEF_NOTIFY_RESURRECTPLAYER = 0x0BE9;
static inline constexpr uint16_t DEF_NOTIFY_HELDENIANTELEPORT = 0x0BE6;
static inline constexpr uint16_t DEF_NOTIFY_0BE8 = 0x0BE8;

static inline constexpr uint32_t MSGID_ITEMCONFIGURATIONCONTENTS = 0x0FA314D9;
static inline constexpr uint32_t MSGID_NPCCONFIGURATIONCONTENTS = 0x0FA314DA;
static inline constexpr uint32_t MSGID_MAGICCONFIGURATIONCONTENTS = 0x0FA314DB;
static inline constexpr uint32_t MSGID_SKILLCONFIGURATIONCONTENTS = 0x0FA314DC;
static inline constexpr uint32_t MSGID_PLAYERITEMLISTCONTENTS = 0x0FA314DD;
static inline constexpr uint32_t MSGID_POTIONCONFIGURATIONCONTENTS = 0x0FA314DE;
static inline constexpr uint32_t MSGID_PLAYERCHARACTERCONTENTS = 0x0FA40000;
static inline constexpr uint32_t MSGID_QUESTCONFIGURATIONCONTENTS = 0x0FA40001;
static inline constexpr uint32_t MSGID_BUILDITEMCONFIGURATIONCONTENTS = 0x0FA40002;
static inline constexpr uint32_t MSGID_DUPITEMIDFILECONTENTS = 0x0FA40003;
static inline constexpr uint32_t MSGID_NOTICEMENTFILECONTENTS = 0x0FA40004;
//static inline constexpr uint32_t WorldCfg = 0x0FA40005;
static inline constexpr uint32_t MSGID_NPCITEMCONFIGCONTENTS = 0x0FA40006;
//static inline constexpr uint32_t WLServer = 0x0FA40007;
//static inline constexpr uint32_t Teleport = 0x0FA40008;
//static inline constexpr uint32_t ApocalypeSchedule = 0x0FA40009;
//static inline constexpr uint32_t HeldinianSchedule = 0x0FA40010;

static inline constexpr uint32_t MSGID_COMMAND_CHECKCONNECTION = 0x03203203;
static inline constexpr uint32_t MSGID_COMMAND_CHATMSG = 0x03203204;

static inline constexpr uint32_t MSGID_REQUEST_PING = 0x0FFAACCA;
static inline constexpr uint32_t MSGID_RESPONSE_PING = 0x0FFAACCB;
static inline constexpr uint32_t MSGID_REQUEST_FLOORSTATS = 0x0FFAACCC;
static inline constexpr uint32_t MSGID_RESPONSE_FLOORSTATS = 0x0FFAACCD;
static inline constexpr uint32_t MSGID_REQUEST_CHARLIST = 0x0FFAACCE;
static inline constexpr uint32_t MSGID_RESPONSE_CHARLIST = 0x0FFAACCF;
static inline constexpr uint32_t MSGID_REQUEST_FRIENDSLIST = 0x0FFAACD1;
static inline constexpr uint32_t MSGID_RESPONSE_FRIENDSLIST = 0x0FFAACD2;
static inline constexpr uint32_t MSGID_REQUEST_REGISTER = 0x0FFAACD3;
static inline constexpr uint32_t MSGID_RESPONSE_REGISTER = 0x0FFAACD4;
static inline constexpr uint32_t MSGID_REQUEST_PKLIST = 0x0FFAACD5;
static inline constexpr uint32_t MSGID_RESPONSE_PKLIST = 0x0FFAACD6;

static inline constexpr uint32_t MSGID_REQUEST_REGISTERGAMESERVER = 0x0512A3F4;
static inline constexpr uint32_t MSGID_RESPONSE_REGISTERGAMESERVER = 0x0512A3F5;
static inline constexpr uint32_t MSGID_REQUEST_REGISTERDBSERVER = 0x0512A3F6;
static inline constexpr uint32_t MSGID_RESPONSE_REGISTERDBSERVER = 0x0512A3F7;

static inline constexpr uint32_t MSGID_REQUEST_LOGIN = 0x0FC94201;
static inline constexpr uint32_t MSGID_REQUEST_CREATENEWACCOUNT = 0x0FC94202;
static inline constexpr uint32_t MSGID_RESPONSE_LOG = 0x0FC94203;
static inline constexpr uint32_t MSGID_REQUEST_CREATENEWCHARACTER = 0x0FC94204;
static inline constexpr uint32_t MSGID_REQUEST_ENTERGAME = 0x0FC94205;
static inline constexpr uint32_t MSGID_RESPONSE_ENTERGAME = 0x0FC94206;
static inline constexpr uint32_t MSGID_REQUEST_DELETECHARACTER = 0x0FC94207;
static inline constexpr uint32_t MSGID_REQUEST_CREATENEWGUILD = 0x0FC94208;
static inline constexpr uint32_t MSGID_RESPONSE_CREATENEWGUILD = 0x0FC94209;
static inline constexpr uint32_t MSGID_REQUEST_DISBANDGUILD = 0x0FC9420A;
static inline constexpr uint32_t MSGID_RESPONSE_DISBANDGUILD = 0x0FC9420B;
static inline constexpr uint32_t MSGID_REQUEST_HELDENIAN_WINNER = 0x3D001240;

static inline constexpr uint32_t MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN = 0x0FC9420C;
static inline constexpr uint32_t MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN = 0x0FC9420D;

static inline constexpr uint32_t MSGID_REQUEST_CIVILRIGHT = 0x0FC9420E;
static inline constexpr uint32_t MSGID_RESPONSE_CIVILRIGHT = 0x0FC9420F;

static inline constexpr uint32_t MSGID_REQUEST_CHANGEPASSWORD = 0x0FC94210;
static inline constexpr uint32_t MSGID_RESPONSE_CHANGEPASSWORD = 0x0FC94211;

static inline constexpr uint16_t DEF_LOGRESMSGTYPE_CONFIRM = 0x0F14;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_REJECT = 0x0F15;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_PASSWORDMISMATCH = 0x0F16;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NOTEXISTINGACCOUNT = 0x0F17;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED = 0x0F18;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED = 0x0F19;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT = 0x0F1A;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NOTEXISTINGCHARACTER = 0x0F1B;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED = 0x0F1C;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NEWCHARACTERFAILED = 0x0F1D;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER = 0x0F1E;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_CHARACTERDELETED = 0x0F1F
;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_ACCOUNTLOCKED = 0x0F31
;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_SERVICENOTAVAILABLE = 0x0F32;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS = 0x0A00;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL = 0x0A01;
static inline constexpr uint16_t DEF_LOGRESMSGTYPE_NOTEXISTINGWORLDSERVER = 0x0A02;

static inline constexpr uint16_t DEF_ENTERGAMEMSGTYPE_NEW = 0x0F1C;
static inline constexpr uint16_t DEF_ENTERGAMEMSGTYPE_NOENTER_FORCEDISCONN = 0x0F1D;
static inline constexpr uint16_t DEF_ENTERGAMEMSGTYPE_CHANGINGSERVER = 0x0F1E;

static inline constexpr uint16_t DEF_ENTERGAMERESTYPE_PLAYING = 0x0F20;
static inline constexpr uint16_t DEF_ENTERGAMERESTYPE_REJECT = 0x0F21;
static inline constexpr uint16_t DEF_ENTERGAMERESTYPE_CONFIRM = 0x0F22;
static inline constexpr uint16_t DEF_ENTERGAMERESTYPE_FORCEDISCONN = 0x0F23;

static inline constexpr uint32_t MSGID_REQUEST_PLAYERDATA = 0x0C152210; // 0x0F1255CA
static inline constexpr uint32_t MSGID_RESPONSE_PLAYERDATA = 0x0C152211; // 0x0F88CCAB
static inline constexpr uint32_t MSGID_RESPONSE_SAVEPLAYERDATA_REPLY = 0x0C152212; // 0x0F1283BA
static inline constexpr uint32_t MSGID_REQUEST_SAVEPLAYERDATA = 0x0DF3076F; // 0x0F81FFAC
static inline constexpr uint32_t MSGID_REQUEST_SAVEPLAYERDATA_REPLY = 0x0DF30770;
static inline constexpr uint32_t MSGID_REQUEST_SAVEPLAYERDATALOGOUT = 0x0DF3074F; // 0x0F44B1CC
static inline constexpr uint32_t MSGID_REQUEST_NOSAVELOGOUT = 0x0DF30750;


static inline constexpr uint32_t MSGID_REQUEST_RETRIEVEITEM = 0x0DF30751;
static inline constexpr uint32_t MSGID_RESPONSE_RETRIEVEITEM = 0x0DF30752;

static inline constexpr uint32_t MSGID_REQUEST_FULLOBJECTDATA = 0x0DF40000;

static inline constexpr uint32_t MSGID_GUILDNOTIFY = 0x0DF30760;
static inline constexpr uint16_t DEF_GUILDNOTIFY_NEWGUILDSMAN = 0x1F00;

static inline constexpr uint32_t MSGID_REQUEST_TELEPORT = 0x0EA03201;
static inline constexpr uint32_t MSGID_REQUEST_CITYHALLTELEPORT = 0x0EA03202;
static inline constexpr uint32_t MSGID_REQUEST_HELDENIANTELEPORT = 0x0EA03206;

static inline constexpr uint32_t MSGID_LEVELUPSETTINGS = 0x11A01000;
static inline constexpr uint32_t MSGID_STATECHANGEPOINT = 0x11A01001;
static inline constexpr uint32_t MSGID_DYNAMICOBJECT = 0x12A01001;

static inline constexpr uint32_t MSGID_GAMESERVERALIVE = 0x12A01002;
static inline constexpr uint32_t MSGID_ADMINUSER = 0x12A01003;

static inline constexpr uint32_t MSGID_GAMESERVERDOWN = 0x12A01004;
static inline constexpr uint32_t MSGID_TOTALGAMESERVERCLIENTS = 0x12A01005;

static inline constexpr uint32_t MSGID_ENTERGAMECONFIRM = 0x12A01006;

static inline constexpr uint32_t MSGID_REQUEST_FIGHTZONE_RESERVE = 0x12A01007;
static inline constexpr uint32_t MSGID_RESPONSE_FIGHTZONE_RESERVE = 0x12A01008;

static inline constexpr uint32_t DEF_MSGID_ANNOUNCEACCOUNT = 0x13000000;

static inline constexpr uint32_t MSGID_ACCOUNTINFOCHANGE = 0x13000001;
static inline constexpr uint32_t MSGID_IPINFOCHANGE = 0x13000002;

static inline constexpr uint32_t MSGID_GAMESERVERSHUTDOWNED = 0x14000000;
static inline constexpr uint32_t MSGID_ANNOUNCEACCOUNTNEWPASSWORD = 0x14000010;

static inline constexpr uint32_t MSGID_REQUEST_IPIDSTATUS = 0x14E91200;
static inline constexpr uint32_t MSGID_RESPONSE_IPIDSTATUS = 0x14E91201;
static inline constexpr uint32_t MSGID_REQUEST_ACCOUNTCONNSTATUS = 0x14E91202;
static inline constexpr uint32_t MSGID_RESPONSE_ACCOUNTCONNSTATUS = 0x14E91203;
static inline constexpr uint32_t MSGID_REQUEST_CLEARACCOUNTCONNSTATUS = 0x14E91204;
static inline constexpr uint32_t MSGID_RESPONSE_CLEARACCOUNTCONNSTATUS = 0x14E91205;

static inline constexpr uint32_t MSGID_REQUEST_FORCEDISCONECTACCOUNT = 0x15000000;
static inline constexpr uint32_t MSGID_REQUEST_NOCOUNTINGSAVELOGOUT = 0x15000001;

static inline constexpr uint32_t MSGID_OCCUPYFLAGDATA = 0x167C0A30;
static inline constexpr uint32_t MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA = 0x167C0A31;
static inline constexpr uint32_t MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA = 0x167C0A32;

static inline constexpr uint32_t MSGID_ARESDENOCCUPYFLAGSAVEFILECONTENTS = 0x17081034;
static inline constexpr uint32_t MSGID_ELVINEOCCUPYFLAGSAVEFILECONTENTS = 0x17081035;

static inline constexpr uint32_t MSGID_REQUEST_SETITEMPOS = 0x180ACE0A;

static inline constexpr uint32_t MSGID_BWM_INIT = 0x19CC0F82;
static inline constexpr uint32_t MSGID_BWM_COMMAND_SHUTUP = 0x19CC0F84;

static inline constexpr uint32_t MSGID_SENDSERVERSHUTDOWNMSG = 0x20000000;
static inline constexpr uint32_t MSGID_ITEMLOG = 0x210A914D;
static inline constexpr uint32_t MSGID_GAMEMASTERLOG = 0x210A914E;

static inline constexpr uint32_t MSGID_REQUEST_NOTICEMENT = 0x220B2F00;
static inline constexpr uint32_t MSGID_RESPONSE_NOTICEMENT = 0x220B2F01;

static inline constexpr uint32_t MSGID_REGISTER_WORLDSERVER = 0x23AA210E;
static inline constexpr uint32_t MSGID_REGISTER_WORLDSERVERSOCKET = 0x23AA210F;
static inline constexpr uint32_t MSGID_REGISTER_WORLDSERVER_GAMESERVER = 0x23AB211F;

static inline constexpr uint32_t MSGID_REQUEST_CHARINFOLIST = 0x23AB2200;
static inline constexpr uint32_t MSGID_RESPONSE_CHARINFOLIST = 0x23AB2201;

static inline constexpr uint32_t MSGID_REQUEST_REMOVEGAMESERVER = 0x2400000A;
static inline constexpr uint32_t MSGID_REQUEST_CLEARACCOUNTSTATUS = 0x24021EE0;

static inline constexpr uint32_t MSGID_REQUEST_SETACCOUNTINITSTATUS = 0x25000198;
static inline constexpr uint32_t MSGID_REQUEST_SETACCOUNTWAITSTATUS = 0x25000199;

static inline constexpr uint32_t MSGID_REQUEST_CHECKACCOUNTPASSWORD = 0x2654203A;
static inline constexpr uint32_t MSGID_WORLDSERVERACTIVATED = 0x27049D0C;

static inline constexpr uint32_t MSGID_REQUEST_PANNING = 0x27B314D0;
static inline constexpr uint32_t MSGID_RESPONSE_PANNING = 0x27B314D1;

static inline constexpr uint32_t MSGID_REQUEST_RESTART = 0x28010EEE;
static inline constexpr uint32_t MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET = 0x280120A0;

static inline constexpr uint32_t MSGID_REQUEST_BLOCKACCOUNT = 0x2900AD10;
static inline constexpr uint32_t MSGID_IPTIMECHANGE = 0x2900AD20;
static inline constexpr uint32_t MSGID_ACCOUNTTIMECHANGE = 0x2900AD22;
static inline constexpr uint32_t MSGID_REQUEST_IPTIME = 0x2900AD30;
static inline constexpr uint32_t MSGID_RESPONSE_IPTIME = 0x2900AD31;

static inline constexpr uint32_t MSGID_REQUEST_SELLITEMLIST = 0x2900AD30;

static inline constexpr uint32_t MSGID_REQUEST_ALL_SERVER_REBOOT = 0x3AE8270A;
static inline constexpr uint32_t MSGID_REQUEST_EXEC_1DOTBAT = 0x3AE8370A;
static inline constexpr uint32_t MSGID_REQUEST_EXEC_2DOTBAT = 0x3AE8470A;
static inline constexpr uint32_t MSGID_MONITORALIVE = 0x3AE8570A;

static inline constexpr uint32_t MSGID_COLLECTEDMANA = 0x3AE90000;
static inline constexpr uint32_t MSGID_METEORSTRIKE = 0x3AE90001;

static inline constexpr uint32_t MSGID_SERVERSTOCKMSG = 0x3AE90013;

static inline constexpr uint16_t GSM_REQUEST_FINDCHARACTER = 0x01;
static inline constexpr uint16_t GSM_RESPONSE_FINDCHARACTER = 0x02;
static inline constexpr uint16_t GSM_GRANDMAGICRESULT = 0x03;
static inline constexpr uint16_t GSM_GRANDMAGICLAUNCH = 0x04;
static inline constexpr uint16_t GSM_COLLECTEDMANA = 0x05;
static inline constexpr uint16_t GSM_BEGINCRUSADE = 0x06;
static inline constexpr uint16_t GSM_ENDCRUSADE = 0x07;
static inline constexpr uint16_t GSM_MIDDLEMAPSTATUS = 0x08;
static inline constexpr uint16_t GSM_SETGUILDTELEPORTLOC = 0x09;
static inline constexpr uint16_t GSM_CONSTRUCTIONPOINT = 0x0A;
static inline constexpr uint16_t GSM_SETGUILDCONSTRUCTLOC = 0x0B;
static inline constexpr uint16_t GSM_CHATMSG = 0x0C;
static inline constexpr uint16_t GSM_WHISFERMSG = 0x0D;
static inline constexpr uint16_t GSM_DISCONNECT = 0x0E;
static inline constexpr uint16_t GSM_REQUEST_SUMMONPLAYER = 0x0F;
static inline constexpr uint16_t GSM_REQUEST_SHUTUPPLAYER = 0x10;
static inline constexpr uint16_t GSM_RESPONSE_SHUTUPPLAYER = 0x11;
static inline constexpr uint16_t GSM_REQUEST_SETFORCERECALLTIME = 0x12;
static inline constexpr uint16_t GSM_BEGINAPOCALYPSE = 0x13;
static inline constexpr uint16_t GSM_ENDAPOCALYPSE = 0x14;
static inline constexpr uint16_t GSM_REQUEST_SUMMONGUILD = 0x15;
static inline constexpr uint16_t GSM_REQUEST_SUMMONALL = 0x16;
static inline constexpr uint16_t GSM_ENDHELDENIAN = 0x17;
static inline constexpr uint16_t GSM_UPDATECONFIGS = 0x18;
static inline constexpr uint16_t GSM_STARTHELDENIAN = 0x19;

static inline constexpr uint16_t DEF_COMMONTYPE_REQ_CHANGEPLAYMODE = 0x0A60;
static inline constexpr uint16_t DEF_NOTIFY_CHANGEPLAYMODE = 0x0BA9;

static inline constexpr uint32_t MSGID_PARTYOPERATION = 0x3C00123A;
static inline constexpr uint16_t DEF_PARTYSTATUS_PROCESSING = 1;
static inline constexpr uint16_t DEF_PARTYSTATUS_NULL = 0;
static inline constexpr uint16_t DEF_PARTYSTATUS_CONFIRM = 2;

static inline constexpr uint16_t DEF_COMMONTYPE_UPGRADEITEM = 0x0A58;
static inline constexpr uint16_t DEF_ITEMLOG_UPGRADESUCCESS = 30;
static inline constexpr uint16_t DEF_ITEMLOG_UPGRADEFAIL = 29;
static inline constexpr uint16_t DEF_COMMONTYPE_REQGUILDNAME = 0x0A59;
static inline constexpr uint16_t DEF_COMMONTYPE_REQPLAYERNAME = 0x0AEF;
static inline constexpr uint16_t DEF_COMMONTYPE_REQUEST_HUNTMODE = 0x0A60;
static inline constexpr uint16_t DEF_COMMONTYPE_REQ_CREATESLATE = 0x0A61;

static inline constexpr uint32_t MSGID_GAMEITEMLOG = 0x210A914F;

static inline constexpr uint16_t DEF_CRUSADELOG_ENDCRUSADE = 1;
static inline constexpr uint16_t DEF_CRUSADELOG_STARTCRUSADE = 2;
static inline constexpr uint16_t DEF_CRUSADELOG_SELECTDUTY = 3;
static inline constexpr uint16_t DEF_CRUSADELOG_GETEXP = 4;
static inline constexpr uint32_t MSGID_GAMECRUSADELOG = 0x210A914F;

static inline constexpr uint16_t DEF_ITEMSPREAD_RANDOM = 1;
static inline constexpr uint16_t DEF_ITEMSPREAD_FIXED = 2;
static inline constexpr uint16_t MAX_NPCITEMDROP = 95;

static inline constexpr uint16_t DEF_NOTIFY_SLATECLEAR = 99;

static inline constexpr uint16_t DEF_PKLOG_REDUCECRIMINAL = 1;
static inline constexpr uint16_t DEF_PKLOG_BYPLAYER = 2;
static inline constexpr uint16_t DEF_PKLOG_BYPK = 3;
static inline constexpr uint16_t DEF_PKLOG_BYENERMY = 4;
static inline constexpr uint16_t DEF_PKLOG_BYNPC = 5;
static inline constexpr uint16_t DEF_PKLOG_BYOTHER = 6;

static inline constexpr uint32_t DEF_REQUEST_RESURRECTPLAYER_NO = 0x0FC94215;
static inline constexpr uint32_t DEF_REQUEST_RESURRECTPLAYER_YES = 0x0FC94214;

static inline constexpr uint16_t DEF_NOTIFY_HELDENIANEND = 0x0BE7; // Heldenian holy war has been closed.
static inline constexpr uint16_t DEF_NOTIFY_HELDENIANSTART = 0x0BEA; // Heldenian real battle has been started form now on.
static inline constexpr uint16_t DEF_NOTIFY_HELDENIANVICTORY = 0x0BEB; // HELDENIAN FINISHED FOR Elvine/aresden
static inline constexpr uint16_t DEF_NOTIFY_HELDENIANCOUNT = 0x0BEC; // Sends client number of destroyed towers, deaths....

static inline constexpr uint16_t DEF_NOTIFY_SLATE_BERSERK = 0x0BED; // Berserk magic casted!
static inline constexpr uint16_t DEF_NOTIFY_LOTERY_LOST = 0x0BEE; // You draw a blank. Please try again next time..
static inline constexpr uint16_t DEF_NOTIFY_0BEF = 0x0BEF; // Strange behavior, exits client?
static inline constexpr uint16_t DEF_NOTIFY_CRAFTING_SUCCESS = 0x0BF0; // Crafting ok
static inline constexpr uint16_t DEF_NOTIFY_CRAFTING_FAIL = 0x0BF1; // Crafting failed

static inline constexpr uint16_t DEF_NOTIFY_ANGELIC_STATS = 0x0BF2; // Sends m_iAngelicStr, Int, Dex, Mag to client
static inline constexpr uint16_t DEF_NOTIFY_ITEM_CANT_RELEASE = 0x0BF3; // "Item cannot be released"
static inline constexpr uint16_t DEF_NOTIFY_ANGEL_FAILED = 0x0BF4; // Failed receiving an Angel pendent
static inline constexpr uint16_t DEF_NOTIFY_ANGEL_RECEIVED = 0x0BF5; // "You have received the Tutelary Angel"

static inline constexpr uint32_t MSGID_REQUEST_TELEPORT_LIST = 0x0EA03202;
static inline constexpr uint32_t MSGID_RESPONSE_TELEPORT_LIST = 0x0EA03203;
static inline constexpr uint32_t MSGID_REQUEST_CHARGED_TELEPORT = 0x0EA03204;
static inline constexpr uint32_t MSGID_RESPONSE_CHARGED_TELEPORT = 0x0EA03205;

static inline constexpr uint16_t DEF_NOTIFY_SPELL_SKILL = 0x0BF6;
static inline constexpr uint16_t MSGID_GMLIST = 0x0BFC;
static inline constexpr uint16_t DEF_NOTIFY_CHANGEVALUE = 0x0BFD;
static inline constexpr uint16_t DEF_NOTIFY_SERVERCHAT = 0x0BFE;
static inline constexpr uint16_t DEF_NOTIFY_CONTRIBUTION = 0x0BFF;


static inline constexpr uint16_t DEF_CV_HP = 0x0001;
static inline constexpr uint16_t DEF_CV_MP = 0x0002;
static inline constexpr uint16_t DEF_CV_SP = 0x0003;
static inline constexpr uint16_t DEF_CV_EK = 0x0004;
static inline constexpr uint16_t DEF_CV_PK = 0x0005;
static inline constexpr uint16_t DEF_CV_REP = 0x0006;

static inline constexpr uint32_t MSGID_REQUEST_HELDENIAN_TP_LIST = 0x0EA03206;
static inline constexpr uint32_t MSGID_RESPONSE_HELDENIAN_TP_LIST = 0x0EA03207;
static inline constexpr uint32_t MSGID_REQUEST_HELDENIAN_TP = 0x0EA03208;
static inline constexpr uint32_t MSGID_REQUEST_HELDENIAN_SCROLL = 0x3D001244;

static inline constexpr uint32_t DEF_REQUEST_ANGEL = 0x0FC9421E;

static inline constexpr uint16_t DEF_STR = 0x01;
static inline constexpr uint16_t DEF_DEX = 0x03;
static inline constexpr uint16_t DEF_INT = 0x04;
static inline constexpr uint16_t DEF_VIT = 0x02;
static inline constexpr uint16_t DEF_MAG = 0x05;
static inline constexpr uint16_t DEF_CHR = 0x06;

static inline constexpr uint32_t MSGID_FRIENDSLIST = 0x12345678;
