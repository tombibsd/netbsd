.\"	$NetBSD$
.br
.po 10
.if n \!.
.sp 15
.tr ^ \"
.ce 88
^****^^^^*****^^^^^^*^^^^^^****^
*^^^^^^^^^^*^^^^^^^*^*^^^^^*^^^*
^***^^^^^^^*^^^^^^*****^^^^****^
^^^^*^^^^^^*^^^^^^*^^^*^^^^*^^*^
****^^^^^^^*^^^^^^*^^^*^^^^*^^^*


*****^^^^****^^^^^*****^^^^*^^^*
^^*^^^^^^*^^^*^^^^*^^^^^^^^*^^*^
^^*^^^^^^****^^^^^***^^^^^^***^^
^^*^^^^^^*^^*^^^^^*^^^^^^^^*^^*^
^^*^^^^^^*^^^*^^^^*****^^^^*^^^*


by

Eric Allman
University of California
Berkeley
.ce 0
.tr ^^
.de HE
'sp 4
'tl 'STAR TREK''%'
'sp 3
..
.de FO
'bp
..
.wh 0 HE
.wh -5 FO
.de pp
.sp
.ti +4
..
.bp 1
.ce
INTRODUCTION
.pp
Well, the federation is once again at war with the Klingon empire.
It is up to you,
as captain of the U.S.S. Enterprise,
to wipe out the invasion fleet and save the Federation.
.pp
For the purposes of the game
the galaxy is divided into 64 quadrants
on an eight by eight grid,
with quadrant 0,0 in the upper left hand corner.
Each quadrant is divided into 100 sectors
on a ten by ten grid.
Each sector contains one object
(e.g., the Enterprise, a Klingon, or a star).
.pp
Navigation is handled in degrees,
with zero being straight up
and ninty being to the right.
Distances are measured in quadrants.
One tenth quadrant is one sector.
.pp
The galaxy contains starbases,
at which you can dock to refuel,
repair damages, etc.
The galaxy also contains stars.
Stars usually have a knack for getting in your way,
but they can be triggered into going nova
by shooting a photon torpedo at one,
thereby (hopefully) destroying any adjacent Klingons.
This is not a good practice however,
because you are penalized for destroying stars.
Also, a star will sometimes go supernova,
which obliterates an entire quadrant.
You must never stop in a supernova quadrant,
although you may "jump over" one.
.pp
Some starsystems
have inhabited planets.
Klingons can attack inhabited planets
and enslave the populace,
which they then put to work building more Klingon battle cruisers.
.bp
.ce
STARTING UP THE GAME
.pp
To request the game, issue the command
.sp
.ti +12
/usr/games/trek
.sp
from the shell.
If a filename is stated,
a log of the game is written
onto that file.
If omitted,
the file is not written.
If the "-a" flag is stated before the filename,
that file is appended to
rather than created.
.pp
The game will ask you what length game
you would like.
Valid responses are "short", "medium", and "long".
Ideally the length of the game does not
affect the difficulty,
but currently the shorter games
tend to be harder than the longer ones.
You may also type "restart",
which restarts a previously saved game.
.pp
You will then be prompted for the skill,
to which you must respond
"novice", "fair", "good", "expert",
"commadore", or "impossible".
You should start out with a novice
and work up,
but if you really want to see how fast
you can be slaughtered,
start out with an impossible game.
.pp
In general,
throughout the game,
if you forget what is appropriate
the game will tell you what it expects
if you just type in
a question mark.
.pp
To get a copy of these rules,
execute the command
.sp
.ti +12
nroff /usr/games/trekmanual.nr
.sp
.bp
.ce
ISSUING COMMANDS
.pp
If the game expects you to enter a command,
.hc ^
it will say ^"Command:\ "
and wait for your response.
Most commands can be abbreviated.
.pp
At almost any time you can type more than one thing on a line.
For example,
to move straight up one quadrant,
you can type
.ti +12
move 0 1
.br
or you could just type
.ti +12
move
.br
and the game would prompt you with
.ti +12
Course:
.br
to which you could type
.ti +12
0 1
.br
The "1" is the distance,
which could be put on still another line.
Also, the "move" command
could have been abbreviated
"mov", "mo", or just "m".
.pp
If you are partway through a command
and you change your mind,
you can usually type "-1"
to cancel the command.
.pp
Klingons generally cannot hit you
if you don't consume anything
(e.g., time or energy),
so some commands are considered "free".
As soon as you consume anything though -- POW!
.bp
.de **
.if \\n+l .**
.as x *
..
.de bl
.nr l \\w'\\$1' -\\w'*'
.ds x ****
.**
.sp 3
.ne 3
\\*x
.br
.if t *\h'\w'*'u'\fB\\$1\fP\h'\w'*'u'*
.if n * \\$1 *
.br
\\*x
.sp
.in +8
.nf
..
.de FF
.in -8
.fi
..
.if !\n(.V .ta \w'Full Commands: '+1
.if \n(.V .ta \w'Full Commands: 'u
.ce
THE COMMANDS
.bl "Short Range Scan"
Mnemonic: srscan
Shortest Appreviation: s
Full Commands: srscan
	srscan yes/no
Consumes: nothing
.FF
.pp
The short range scan
gives you a picture
of the quadrant you are in,
and (if you say "yes")
a status report
which tells you
a whole bunch
of interesting stuff.
You can get a status report alone
by using the
.ul
status
command.
An example follows:
.sp
.nf
.in +4
Short range sensor scan
  0 1 2 3 4 5 6 7 8 9 
0 . . . . . . . * . * 0   stardate      3702.16
1 . . E . . . . . . . 1   condition     RED
2 . . . . . . . . . * 2   position      0,3/1,2
3 * . . . . # . . . . 3   warp factor   5.0
4 . . . . . . . . . . 4   total energy  4376
5 . . * . * . . . . . 5   torpedoes     9
6 . . . @ . .   . . . 6   shields       down, 78%
7 . . . . . . . . . . 7   Klingons left 3
8 . . . K . . . . . . 8   time left     6.43
9 . . . . . . * . . . 9   life support  damaged, reserves = 2.4
  0 1 2 3 4 5 6 7 8 9 
Distressed Starsystem Marcus XII

.in +8
.ti -8
The cast of characters is as follows:
E  the hero
K  the villain
#  the starbase
*  stars
@  inhabited starsystem
\&.  empty space
   a black hole
.in -12
.fi
.pp
The name of the starsystem is listed underneath
the short range scan.
The word "distressed", if present,
means that the starsystem
is under attack.
.pp
Short range scans are absolutely free.
They use no time, no energy,
and they don't give the Klingons
another chance to hit you.
.bl "Status Report"
Mnemonic: status
Shortest Abbreviation: st
Consumes: nothing
.FF
.pp
This command gives you information
about the current status
of the game and your ship, as follows:
.in +8
.de qq
.sp
.ti -4
..
.qq
Stardate -- The current stardate.
.qq
Condition -- as follows:
.in +4
.nf
RED -- in battle
YELLOW -- low on energy
GREEN -- normal state
DOCKED -- docked at starbase
CLOAKED -- the cloaking device is activated
.fi
.in -4
.qq
Position -- Your current quadrant and sector.
.qq
Warp Factor -- The speed you will move at
when you move under warp power
(with the
.ul
move
command).
.qq
Total Energy -- Your energy reserves.
If they drop to zero,
you die.
Energy regenerates,
but the higher the skill of the game,
the slower it regenerates.
.qq
Torpedoes -- How many photon torpedoes you have left.
.qq
Shields -- Whether your shields are up or down,
and how effective they are if up
(what percentage of a hit they will absorb).
.qq
Klingons Left -- Guess.
.qq
Time Left -- How long the Federation can hold out
if you sit on your fat ass and do nothing.
If you kill Klingons quickly,
this number goes up,
otherwise,
it goes down.
If it hits zero,
the Federation is conquered.
.qq
Life Support -- If "active", everything is fine.
If "damaged", your reserves tell you
how long you have
to repair your life support
or get to a starbase
before you starve, suffocate,
or something equally unpleasant.
.qq
Current Crew -- The number of crew members
left.
This figures does not include officers.
.qq
Brig Space -- The space left in your brig
for Klingon captives.
.qq
Klingon Power -- The number of units
needed to kill a Klingon.
Remember, as Klingons fire at you
they use up their own energy,
so you probably need somewhat less
than this.
.qq
Skill, Length -- The skill and length
of the game you are playing.
.in -8
.pp
Status information is absolutely free.
.bl "Long Range Scan"
Mnemonic: lrscan
Shortest Abbreviation: l
Consumes: nothing
.FF
.pp
Long range scan gives you information about the
eight quadrants
that surround the quadrant
you're in.
A sample long range scan follows:
.sp
.in +12
.nf
Long range scan for quadrant 0,3

     2     3     4
  -------------------
  !  *  !  *  !  *  !
  -------------------
0 ! 108 !   6 !  19 !
  -------------------
1 !   9 ! /// !   8 !
  -------------------
.sp
.in -12
.fi
.pp
The three digit numbers
tell the number of objects
in the quadrants.
The units digit tells the number of stars,
the tens digit the number of starbases,
and the hundreds digit is the number of Klingons.
"*" indicates the negative energy barrier
at the edge of the galaxy,
which you cannot enter.
"///" means that that is a supernova quadrant
and must not be entered.
.bl "Damage Report"
Mnemonic: damages
Shortest Abbreviation: da
Consumes: nothing
.FF
.pp
A damage report tells you what devices are damaged
and how long it will take to repair them.
Repairs proceed faster
when you are docked
at a starbase.
.bl "Set Warp Factor"
Mnemonic: warp
Shortest Abbreviation: w
Full Command: warp factor
Consumes: nothing
.FF
.pp
The warp factor tells the speed of your starship
when you move under warp power
(with the
.ul
move
command).
The higher the warp factor,
the faster you go,
and the more energy you use.
.pp
The minimum warp factor is 1.0
and the maximum is 10.0.
At speeds above warp 6
there is danger of the warp engines
being damaged.
The probability of this
increases at higher warp speeds.
Above warp 9.0 there is a chance of entering
a time warp.
.bl "Move Under Warp Power"
Mnemonic: move
Shortest Abbreviation: m
Full Command: move course distance
Consumes: time and energy
.FF
.pp
This is the usual way of moving.
The course is in degrees and the distance is in quadrants.
To move one sector specify a distance of 0.1.
.pp
Time is consumed proportionately to
the inverse of the warp factor squared,
and directly to the distance.
Energy is consumed as the warp factor cubed,
and directly to the distance.
If you move with your shields up
it doubles the amount of energy consumed.
.pp
When you move in a quadrant containing Klingons,
they get a chance to attack you.
.pp
The computer detects navigation errors.
If the computer is out,
you run the risk of running into things.
.pp
The course is determined by the
Space Inertial Navigation System
[SINS].
As described in
Star Fleet Technical Order TO:02:06:12,
the SINS is calibrated,
after which it becomes the base for navigation.
If damaged,
navigation becomes inaccurate.
When it is fixed,
Spock recalibrates it,
however,
it cannot be calibrated extremely accurately
until you dock at starbase.
.bl "Move Under Impulse Power"
Mnemonic: impulse
Shortest Abbreviation: i
Full Command: impulse course distance
Consumes: time and energy
.FF
.pp
The impulse engines give you a chance to maneuver
when your warp engines are damaged;
however, they are incredibly slow
(0.095 quadrants/stardate).
They require 20 units of energy to engage,
and ten units per sector to move.
.pp
The same comments about the computer and the SINS
apply as above.
.pp
There is no penalty to move under impulse power
with shields up.
.bl "Deflector Shields"
Mnemonic: shields
Shortest Abbreviation: sh
Full Command: shields up/down
Consumes: energy
.FF
.pp
Shields protect you from Klingon attack
and nearby novas.
As they protect you,
they weaken.
A shield which is 78% effective
will absorb 78% of a hit
and let 22% in to hurt you.
.pp
The Klingons have a chance to attack you
every time you raise or lower shields.
Shields do not rise and lower
instantaneously,
so the hit you receive
will be computed with the shields
at an intermediate effectiveness.
.pp
It takes energy to raise shields,
but not to drop them.
.bl "Cloaking Device"
Mnemonic: cloak
Shortest Abbreviation: cl
Full Command: cloak up/down
Consumes: energy
.FF
.pp
When you are cloaked,
Klingons cannot see you,
and hence they do not fire at you.
They are useful for entering
a quadrant
and selecting a good position,
however,
weapons cannot be fired through
the cloak
due to the huge energy drain
that it requires.
.pp
The cloak up command
only starts the cloaking process;
Klingons will continue
to fire at you
until you do something
which consumes time.
.bl "Fire Phasers"
Mnmemonic: phasers
Shortest Abbreviation: p
Full Commands: phasers automatic amount
	phasers manual amt1 course1 spread1 ...
Consumes: energy
.FF
.pp
Phasers are energy weapons;
the energy comes from your ship's reserves
("total energy" on a srscan).
It takes about 250 units of hits
to kill a Klingon.
Hits are cumulative as long as you stay
in the quadrant.
.pp
Phasers become less effective
the further from a Klingon you are.
Adjacent Klingons receive about
90% of what you fire,
at five sectors about 60%,
and at ten sectors about 35%.
They have no effect outside of the quadrant.
.pp
Phasers cannot be fired while shields are up;
to do so would fry you.
They have no effect on starbases or stars.
.pp
In automatic mode
the computer decides how to divide up the energy
among the Klingons present;
in manual mode you do that yourself.
.pp
In manual mode firing
you specify a direction,
amount (number of units to fire)
and spread (0 -> 1.0)
for each of the six phaser banks.
A zero amount
terminates the manual input.
.bl "Fire Photon Torpedoes"
Mnemonic: torpedo
Shortest Abbreviation: t
Full Command: torpedo course [yes/no] [burst angle]
Consumes: torpedoes
.FF
.pp
Torpedoes are projectile weapons -- there are no partial hits.
You either hit your target or you don't.
A hit on a Klingon destroys him.
A hit on a starbase destroys that starbase
(woops!).
Hitting a star usually causes it to go nova,
and occasionally supernova.
.pp
Photon torpedoes cannot be aimed precisely.
They can be fired with shields up,
but they get even more random
as they pass through the shields.
.pp
Torpedoes may be fired in bursts of three.
If this is desired,
the burst angle is the angle
between the three shots,
which may vary from one to fifteen.
The word "no"
says that a burst is not wanted;
the word "yes"
(which may be omitted
if stated on the same line as the course)
says that a burst is wanted.
.pp
Photon torpedoes
have no effect
outside the quadrant.
.bl "Onboard Computer Request"
Mnemonic: computer
Shortest Abbreviation: c
Full Command: computer request; request;...
Consumes: nothing
.FF
.pp
The computer command gives you access to the facilities
of the onboard computer,
which allows you to do all sorts of fascinating stuff.
Computer requests are:
.in +8
.qq
score -- Shows your current score.
.qq
course quad/sect -- Computes the course and distance from whereever
you are to the given location.
If you type "course /x,y"
you will be given the course
to sector x,y in the current quadrant.
.qq
move quad/sect -- Identical to the course
request,
except that the move is executed.
.qq
chart -- prints a chart of the known galaxy,
i.e.,
everything that you have seen with a long range scan.
The format is the same as on a long range scan,
except that "..." means
that you don't yet know what is there,
and ".1." means that you know that a starbase
exists, but you don't know anything else.
"$$$" mans the quadrant
that you are currently in.
.qq
trajectory -- prints the course and distance
to all the Klingons in the quadrant.
.qq
warpcost dist warp_factor -- computes the cost in time and energy
to move `dist' quadrants at warp `warp_factor'.
.qq
impcost dist -- same as warpcost for impulse engines.
.qq
pheff range -- tells how effective your phasers are
at a given range.
.qq
distresslist -- gives a list of currently distressed
starbases
and starsystems.
.in -8
.pp
More than one request may be stated
on a line
by seperating them
with semicolons.
.bl "Dock at Starbase"
Mnemonic: dock
Shortest Abbreviation: do
Consumes: nothing
.FF
.pp
You may dock at a starbase
when you are in one of the eight
adjacent sectors.
.pp
When you dock you are resupplied
with energy, photon torpedoes, and life support reserves.
Repairs are also done faster at starbase.
Any prisoners you have taken
are unloaded.
You do not receive points
for taking prisoners
until this time.
.pp
Starbases have their own deflector shields,
so you are safe from attack while docked.
.bl "Undock from Starbase"
Mnemonic: undock
Shortest Abbreviation: u
Consumes: nothing
.FF
.pp
This just allows you to leave starbase
so that you may proceed on your way.
.bl "Rest"
Mnemonic: rest
Shortest Abbreviation: r
Full Command: rest time
Consumes: time
.FF
.pp
This command allows you to rest to repair damages.
It is not advisable to rest while under attack.
.bl "Call Starbase For Help"
Mnemonic: help
Shortest Abbreviation: help
Consumes: nothing
.FF
.pp
You may call starbase for help via your subspace radio.
Starbase has long range transporter beams to get you.
Problem is,
they can't always rematerialize you.
.pp
You should avoid using this command unless absolutely necessary,
for the above reason and because it counts heavily against you
in the scoring.
.bl "Capture Klingon"
Mnemonic: capture
Shortest Abbreviation: ca
Consumes: time
.FF
.pp
You may request that a Klingon surrender
to you.
If he accepts,
you get to take captives
(but only as many as your brig
can hold).
It is good if you do this,
because you get points for captives.
Also,
if you ever get captured,
you want to be sure that the Federation
has prisoners to exchange for you.
.pp
You must go to a starbase
to turn over your prisoners
to Federation authorities.
.bl "Visual Scan"
Mnemonic: visual
Shortest Abbreviation: v
Full Command: visual course
Consumes: time
.FF
.pp
When your short range scanners are out,
you can still see what is out "there"
by doing a visual scan.
Unfortunately,
you can only see three sectors at one time,
and it takes 0.005 stardates to perform.
.pp
The three sectors in the general direction
of the course specified
are examined
and displayed.
.bl "Abandon Ship"
Mnemonic: abandon
Shortest Abbreviation: abandon
Consumes: nothing
.FF
.pp
The officers escape the Enterprise in the shuttlecraft.
If the transporter is working
and there is an inhabitable starsystem
in the area,
the crew beams down,
otherwise you leave them to die.
You are given an old but still usable ship,
the Faire Queene.
.bl "Ram"
Mnemonic: ram
Shortest Abbreviation: ram
Full Command: ram course distance
Consumes: time and energy
.FF
.pp
This command is identical to "move",
except that the computer
doesn't stop you
from making navigation errors.
.pp
You get very nearly slaughtered
if you ram anything.
.bl "Self Destruct"
Mnemonic: destruct
Shortest Abbreviation: destruct
Consumes: everything
.FF
.pp
Your starship is self-destructed.
Chances are you will destroy
any Klingons
(and stars,
and starbases)
left in your quadrant.
.bl "Terminate the Game"
Mnemonic: terminate
Shortest Abbreviation: terminate
Full Command: terminate yes/no
.FF
.pp
Cancels the current game.
No score is computed.
If you answer yes,
a new game will be started,
otherwise trek exits.
.bl "Call the Shell"
Mnemonic: shell
Shortest Abbreviation: shell
.FF
.pp
Temporarily escapes to the shell.
When you log out of the shell
you will return to the game.
.bp
.ce
SCORING
.in +4
.pp
The scoring algorithm is rather complicated.
Basically,
you get points for each Klingon you kill,
for your Klingon per stardate kill rate,
and a bonus if you win the game.
You lose
points for the number of Klingons left
in the galaxy
at the end of the game,
for getting killed,
for each star, starbase, or inhabited starsystem
you destroy,
for calling for help,
and for each casualty you incur.
.pp
You will be promoted
if you play very well.
You will never get a promotion if you
call for help,
abandon the Enterprise,
get killed,
destroy a starbase or inhabited starsystem,
or destroy too many stars.
.bp
.ce
REFERENCE PAGE
.sp 2
.ta 36 56
.nf
.ul
Command	Uses	Consumes

ABANDON	shuttlecraft,	-
	  transporter
CApture	subspace radio	time
CLoak Up/Down	cloaking device	energy
Computer request; request;...	computer	-
DAmages	-	-
DESTRUCT	computer	-
DOck	-	-
HELP	subspace radio	-
Impulse course distance	impulse engines,	time, energy
	 computer, SINS
Lrscan	L.R. sensors	-
Move course distance	warp engines,	time, energy
	  computer, SINS
Phasers Automatic amount	phasers, computer	energy
Phasers Manual amt1 course1 spread1 ...	phasers	energy
Torpedo course [Yes] angle/No	torpedo tubes	torpedoes
RAM course distance	warp engines,	time, energy
	  computer, SINS
Rest time	-	time
SHELL	-	-
SHields Up/Down	shields	energy
Srscan [Yes/No]	S.R. sensors	-
STatus	-	-
TERMINATE Yes/No	-	-
Undock	-	-
Visual course	-	time
Warp warp_factor	-	-
.fi
