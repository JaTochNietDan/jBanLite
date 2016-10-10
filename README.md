# jBanLite
jBanLite is a simple include that will allow server owners to easily have a player banning system using an SQLite database as the storage center for the ban information. It should be compatible with just about every script out there, I've taken steps to ensure that there are no conflicts with other scripts and that it should be as easy to use as possible for newbies.

* Additionally, this ban system does not ban people's nicknames, it will only ban people's IP addresses

## What's in the package?
- The jBanLite include
- An example Filterscript
- An example website

## Instructions
Make sure that the jBanLite include has been put into your "pawno/includes" directory, then simply add the following to the script you wish to use jBanLite in:

```pawn
#include <jBanLite>
```

Next you need to open up your jBanLite.inc file and edit the settings in there accordingly, you will need to at least enter in the details of your MySQL server and database.

Then you simply add the ban/unban functions where you need them and let the include do the work, if you're confused about it at this stage, please look at the Filterscript provided in the download package as an example.

### Functions

```pawn
native jBan(player_banned, player_banner, reason[], time = 0);
// This function will create a new ban for player_banned by player_banner with a reason[] and a time. The time parameter can be used to set the ban length in minutes, it defaults to 0 (permanent).

native jBanIP(ip[], player_banner, reason[], time = 0);
// This function will create a new ban with the IP you specify. Everything else is the same as jBan.

native jUnbanName(name[], bool:expired = false);
//This function will remove a ban from the list with the name you specify. The expired parameter will specify whether or not you want to remove bans that have expired, it defaults to false, which will not remove expired bans.

native jUnbanIP(IP[], bool:expired = false);
// This function will remove a ban from the list with the IP you specify. The expired parameter will specify whether or not you want to remove bans that have expired, it defaults to false, which will not remove expired bans.
```

Credits
- Y_Less - sscanf and hooking methods
