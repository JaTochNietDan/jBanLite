/*-----------------------------------------------------------------------------
	Title: 			jBanLite - The SQLite Ban API
	Version:		1
	Author: 		JaTochNietDan (http://www.jatochnietdan.com)
	Description:
					jBanLite allows users to create their own SA-MP PAWN gamemodes
					and filterscripts while being able to use jBan as their
					system for handling bans in an SQLite database. I've done my 
					best to make this include as compatible as possible with 
					other scripts, it uses hooks and other methods as
					prevention against conflicts with other scripts 
					and user generated errors.

	Credits to:
					Y_Less for his sscanf function and hooking methods.
-------------------------------------------------------------------------------*/

#include <a_sampdb>

//--------------------------------------------
// The name of the database
//--------------------------------------------
#define J_DB jban.db

//--------------------------------------------
// The name of the ban table
//--------------------------------------------
#define J_TABLE bans

//--------------------------------------------
// Set this to 1 to enable logging
//--------------------------------------------
#define J_LOGGING 1

//--------------------------------------------
// Only edit below here if you know what you're doing
//--------------------------------------------
#define COLOR_RED 0xFF0000FF

#define MAX_PLAYER_IP 15
#define MAX_QUERY_LENGTH 400
#define MAX_RESULT_LENGTH 74
#define MAX_MESSAGE_LENGTH 128
#define MAX_REASON_LENGTH 40

new 
	DB:jban_db,
	DBResult:jban_result;

new 
	jNames[MAX_PLAYERS][MAX_PLAYER_NAME],
	jIP[MAX_PLAYERS][MAX_PLAYER_IP];

new 
	jBan_Name[MAX_PLAYER_NAME],
	jBan_Reason[MAX_REASON_LENGTH],
	jBan_Message[MAX_MESSAGE_LENGTH],
	jBan_TimeLeft[10],
	jBan_Time[10],
	jQuery[MAX_QUERY_LENGTH];
	
/*-------------------------------------------------------------------
native jBan(player_banned, player_banner, reason[], time = 0);

	Usage:
 			Use this to ban a selected player for a certain amount
			of time.

	Params:
			player_banned - The ID of the player to be banned
			player_banner - The ID of the player banning the player
			reason - The reason for the ban (Optional)
			time - The amount of the ban will last in minutes (Optional)

	Note:
			If a time is not specified, it will default to a
			permanent ban.
-------------------------------------------------------------------*/
stock jBan(player_banned, player_banner, reason[], time = 0)
{
	if(strlen(reason) > MAX_REASON_LENGTH) return SendClientMessage(player_banner, 0xFFFFFF, "[jBan]: Sorry, the maximum reason length is "#MAX_REASON_LENGTH" characters!");

	format(jQuery, MAX_QUERY_LENGTH, "INSERT INTO `"#J_TABLE"` (user_banned, user_banned_ip, user_banner, ban_reason, ban_timestamp, ban_time) VALUES ('%s', '%s', '%s', '%s', CURRENT_TIMESTAMP, %d)", jNames[player_banned], jIP[player_banned], jNames[player_banner], reason, time);
	db_free_result(db_query(jban_db, jQuery));
	
	Kick(player_banned);
	
	#if J_LOGGING == 1
		printf("[jBan]: Player \"%s\" (%s) banned by \"%s\" with reason \"%s\" for %d minutes", jNames[player_banned], jIP[player_banned], jNames[player_banner], reason, time);
	#endif
	return 1;
}

/*-------------------------------------------------------------------
native jBanIP(ip[], player_banner, reason[], time = 0);

	Usage:
 			Use this to ban a selected player for a certain amount
			of time.

	Params:
			ip - The IP to be banned as a string
			player_banner - The ID of the player making the ban
			reason - The reason for the ban (Optional)
			time - The amount of the ban will last in minutes (Optional)

	Note:
			If a time is not specified, it will default to a
			permanent ban.
-------------------------------------------------------------------*/
stock jBanIP(ip[], player_banner, reason[], time = 0)
{
	if(strlen(reason) > MAX_REASON_LENGTH) return 0;
	if(strlen(ip) > MAX_PLAYER_IP) return 0;

	format(jQuery, MAX_QUERY_LENGTH, "INSERT INTO `"#J_TABLE"` (user_banned, user_banned_ip, user_banner, ban_reason, ban_timestamp, ban_time) VALUES ('', '%s', '%s', '%s', CURRENT_TIMESTAMP, %d)", db_escape(ip), jNames[player_banner], db_escape(reason), time);
	db_free_result(db_query(jban_db, jQuery));
	
	#if J_LOGGING == 1
		printf("[jBan]: IP \"%s\" banned by \"%s\" with reason \"%s\" for %d minutes", db_escape(ip), jNames[player_banner], db_escape(reason), time);
	#endif
	return 1;
}

/*-------------------------------------------------------------------
 native jBanCheck(playerid);
 
	Usage:
 			This function will find a player with an un-expired
			ban or a permanent ban and remove them from the
			server with the reason of their ban, the time left
			and the person that banned them.

	Params:	
			playerid - ID of the player to check for ban

	Note:
			This function is already hooked and does not need
			to be called manually for any circumstances.
-------------------------------------------------------------------*/
stock jBanCheck(playerid)
{
	format(jQuery, MAX_QUERY_LENGTH, "SELECT user_banner, ban_reason, ban_time, (strftime('%ss', CURRENT_TIMESTAMP) - strftime('%ss', datetime(ban_timestamp, '+' || ban_time || ' minute')))/60 AS time_left FROM `bans` WHERE user_banned_ip = '%s' AND (time_left < 0 OR ban_time = 0); ", "%", "%", jIP[playerid]);
	jban_result = db_query(jban_db, jQuery);
	
	if(db_num_rows(jban_result) != 0)
	{
		db_get_field(jban_result, 0, jBan_Name, MAX_PLAYER_NAME);
		db_get_field(jban_result, 1, jBan_Reason, MAX_REASON_LENGTH);
		db_get_field(jban_result, 2, jBan_Time, 10);
		db_get_field(jban_result, 3, jBan_TimeLeft, 10);
		
		if(strval(jBan_Time) == 0) format(jBan_Message, MAX_MESSAGE_LENGTH, "[jBan]: %s, you have an existing ban from \"%s\" for \"%s\", it is permanent!", jNames[playerid], jBan_Name, jBan_Reason);
		else format(jBan_Message, MAX_MESSAGE_LENGTH, "[jBan]: %s, you have an existing ban from \"%s\" for \"%s\", it will expire in %d minutes!", jNames[playerid], jBan_Name, jBan_Reason, strval(jBan_Time));
		SendClientMessage(playerid, COLOR_RED, jBan_Message);
		
		#if J_LOGGING == 1
			print(jBan_Message);
		#endif
		
		Kick(playerid);		
	}
	
	db_free_result(jban_result);
	return 1;
}

/*-------------------------------------------------------------------
 native jUnbanName(name[], bool:expired = false);
 
	Usage:
 			This function will remove bans for a specified name.

	Params:	
			name[] - The name of the player as a string.
			bool:expired - Remove expired bans or not.

	Note:
			If expired is not set, it will not removed expired
			bans by default, it will only remove active bans.
-------------------------------------------------------------------*/
stock jUnbanName(name[], bool:expired = false)
{
	if(strlen(name) > MAX_PLAYER_NAME) return 0;
	
	strcat(name, db_escape(name), MAX_PLAYER_NAME);
	
	if(expired == false) format(jQuery, MAX_QUERY_LENGTH, "DELETE FROM `"#J_TABLE"` WHERE user_banned = '%s' AND (strftime('%ss', CURRENT_TIMESTAMP) - strftime('%ss', datetime(ban_timestamp, '+' || ban_time || ' minute')))/60 < 0", name, "%", "%");
	else format(jQuery, MAX_QUERY_LENGTH, "DELETE FROM `"#J_TABLE"` WHERE user_banned = '%s'", name);
	
	db_free_result(db_query(jban_db, jQuery));
	return 1;
}

/*-------------------------------------------------------------------
 native jUnbanIP(IP[], bool:expired = false);
 
	Usage:
 			This function will remove bans for a specified IP address.

	Params:	
			IP[] - The name of the player as a string.
			bool:expired - Remove expired bans or not.

	Note:
			If expired is not set, it will not removed expired
			bans by default, it will only remove active bans.
-------------------------------------------------------------------*/
stock jUnbanIP(IP[], bool:expired = false)
{
	if(strlen(IP) > MAX_PLAYER_IP) return 0;
	
	strcat(IP, db_escape(IP), MAX_PLAYER_IP);
	
	if(expired == false) format(jQuery, MAX_QUERY_LENGTH, "DELETE FROM `"#J_TABLE"` WHERE user_ip = '%s' AND (strftime('%ss', CURRENT_TIMESTAMP) - strftime('%ss', datetime(ban_timestamp, '+' || ban_time || ' minute')))/60 < 0", IP, "%", "%");
	else format(jQuery, MAX_QUERY_LENGTH, "DELETE FROM `"#J_TABLE"` WHERE user_ip = '%s'", IP);
	
	db_free_result(db_query(jban_db, jQuery));
	return 1;
}

stock jban_SetPlayerName(playerid, name[])
{
    format(jNames[playerid], MAX_PLAYER_NAME, name);
    return SetPlayerName(playerid, name);
}

public OnPlayerConnect(playerid)
{
	GetPlayerName(playerid, jNames[playerid], MAX_PLAYER_NAME);
	GetPlayerIp(playerid, jIP[playerid], MAX_PLAYER_IP);
	jBanCheck(playerid);
	return CallLocalFunction("jban_OnPlayerConnect", "i", playerid);
}

public OnGameModeInit() 
{
	jban_db = db_open(""#J_DB"");

	db_free_result(db_query(jban_db,
		"CREATE TABLE IF NOT EXISTS \""#J_TABLE"\" ("\
		"\"ID\"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"\
		"\"user_banned\"  TEXT(24) NOT NULL,"\
		"\"user_banned_ip\"  TEXT(15) NOT NULL,"\
		"\"user_banner\"  TEXT(24) NOT NULL,"\
		"\"ban_reason\"  TEXT(40) NOT NULL,"\
		"\"ban_timestamp\"  TEXT(20) NOT NULL,"\
		"\"ban_time\"  INTEGER NOT NULL);"));
		
	return CallLocalFunction("jban_OnGameModeInit", "");
}

public OnGameModeExit()
{
	db_close(jban_db);
	return CallLocalFunction("jban_OnGameModeExit", "");
}

//---------------------------------
// sscanf by Alex "Y_Less" Cole
// http://www.y-less.com
//---------------------------------
stock j_sscanf(string[], format[], {Float,_}:...)
{
	#if defined isnull
		if (isnull(string))
	#else
		if (string[0] == 0 || (string[0] == 1 && string[1] == 0))
	#endif
		{
			return format[0];
		}
	#pragma tabsize 4
	new
		formatPos = 0,
		stringPos = 0,
		paramPos = 2,
		paramCount = numargs(),
		delim = ' ';
	while (string[stringPos] && string[stringPos] <= ' ')
	{
		stringPos++;
	}
	while (paramPos < paramCount && string[stringPos])
	{
		switch (format[formatPos++])
		{
			case '\0':
			{
				return 0;
			}
			case 'i', 'd':
			{
				new
					neg = 1,
					num = 0,
					ch = string[stringPos];
				if (ch == '-')
				{
					neg = -1;
					ch = string[++stringPos];
				}
				do
				{
					stringPos++;
					if ('0' <= ch <= '9')
					{
						num = (num * 10) + (ch - '0');
					}
					else
					{
						return -1;
					}
				}
				while ((ch = string[stringPos]) > ' ' && ch != delim);
				setarg(paramPos, 0, num * neg);
			}
			case 'h', 'x':
			{
				new
					num = 0,
					ch = string[stringPos];
				do
				{
					stringPos++;
					switch (ch)
					{
						case 'x', 'X':
						{
							num = 0;
							continue;
						}
						case '0' .. '9':
						{
							num = (num << 4) | (ch - '0');
						}
						case 'a' .. 'f':
						{
							num = (num << 4) | (ch - ('a' - 10));
						}
						case 'A' .. 'F':
						{
							num = (num << 4) | (ch - ('A' - 10));
						}
						default:
						{
							return -1;
						}
					}
				}
				while ((ch = string[stringPos]) > ' ' && ch != delim);
				setarg(paramPos, 0, num);
			}
			case 'c':
			{
				setarg(paramPos, 0, string[stringPos++]);
			}
			case 'f':
			{
 
				new changestr[16], changepos = 0, strpos = stringPos;     
				while(changepos < 16 && string[strpos] && string[strpos] != delim)
				{
					changestr[changepos++] = string[strpos++];
    				} 
				changestr[changepos] = '\0'; 
				setarg(paramPos,0,_:floatstr(changestr)); 
			} 
			case 'p':
			{
				delim = format[formatPos++];
				continue;
			}
			case '\'':
			{
				new
					end = formatPos - 1,
					ch;
				while ((ch = format[++end]) && ch != '\'') {}
				if (!ch)
				{
					return -1;
				}
				format[end] = '\0';
				if ((ch = strfind(string, format[formatPos], false, stringPos)) == -1)
				{
					if (format[end + 1])
					{
						return -1;
					}
					return 0;
				}
				format[end] = '\'';
				stringPos = ch + (end - formatPos);
				formatPos = end + 1;
			}
			case 'u':
			{
				new
					end = stringPos - 1,
					id = 0,
					bool:num = true,
					ch;
				while ((ch = string[++end]) && ch != delim)
				{
					if (num)
					{
						if ('0' <= ch <= '9')
						{
							id = (id * 10) + (ch - '0');
						}
						else
						{
							num = false;
						}
					}
				}
				if (num && IsPlayerConnected(id))
				{
					setarg(paramPos, 0, id);
				}
				else
				{
					#if !defined foreach
						#define foreach(%1,%2) for (new %2 = 0; %2 < MAX_PLAYERS; %2++) if (IsPlayerConnected(%2))
						#define __SSCANF_FOREACH__
					#endif
					string[end] = '\0';
					num = false;
					new
						name[MAX_PLAYER_NAME];
					id = end - stringPos;
					foreach (Player, playerid)
					{
						GetPlayerName(playerid, name, sizeof (name));
						if (!strcmp(name, string[stringPos], true, id))
						{
							setarg(paramPos, 0, playerid);
							num = true;
							break;
						}
					}
					if (!num)
					{
						setarg(paramPos, 0, INVALID_PLAYER_ID);
					}
					string[end] = ch;
					#if defined __SSCANF_FOREACH__
						#undef foreach
						#undef __SSCANF_FOREACH__
					#endif
				}
				stringPos = end;
			}
			case 's', 'z':
			{
				new
					i = 0,
					ch;
				if (format[formatPos])
				{
					while ((ch = string[stringPos++]) && ch != delim)
					{
						setarg(paramPos, i++, ch);
					}
					if (!i)
					{
						return -1;
					}
				}
				else
				{
					while ((ch = string[stringPos++]))
					{
						setarg(paramPos, i++, ch);
					}
				}
				stringPos--;
				setarg(paramPos, i, '\0');
			}
			default:
			{
				continue;
			}
		}
		while (string[stringPos] && string[stringPos] != delim && string[stringPos] > ' ')
		{
			stringPos++;
		}
		while (string[stringPos] && (string[stringPos] == delim || string[stringPos] <= ' '))
		{
			stringPos++;
		}
		paramPos++;
	}
	do
	{
		if ((delim = format[formatPos++]) > ' ')
		{
			if (delim == '\'')
			{
				while ((delim = format[formatPos++]) && delim != '\'') {}
			}
			else if (delim != 'z')
			{
				return delim;
			}
		}
	}
	while (delim > ' ');
	return 0;
}

// Thanks to the author of this on the SA-MP Wiki
stock db_escape(text[])
{
	new
		ret[24 * 2],
		ch,
		i,
		j;
	while ((ch = text[i++]) && j < sizeof (ret))
	{
		if (ch == '\'')
		{
			if (j < sizeof (ret) - 2)
			{
				ret[j++] = '\'';
				ret[j++] = '\'';
			}
		}
		else if (j < sizeof (ret))
		{
			ret[j++] = ch;
		}
		else
		{
			j++;
		}
	}
	ret[sizeof (ret) - 1] = '\0';
	return ret;
}

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit jban_OnGameModeInit
#if defined _ALS_OnGameModeExit
    #undef OnGameModeExit
#else
    #define _ALS_OnGameModeExit
#endif
#define OnGameModeExit jban_OnGameModeExit
#if defined _ALS_SetPlayerName
    #undef SetPlayerName
#else
    #define _ALS_SetPlayerName
#endif
#define SetPlayerName jban_SetPlayerName
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect jban_OnPlayerConnect
forward jban_OnGameModeInit();
forward jban_OnGameModeExit();
forward jban_OnQueryFinish(query[], index, extraid, connectionHandle);
forward jban_SetPlayerName(playerid, name[]);
forward jban_OnPlayerConnect(playerid);