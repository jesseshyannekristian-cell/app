"""Per-SCP branching breach mini-scenarios.

Each scenario is a list of decision stages. Each stage offers options; choosing one
applies a delta to the running survival chance and prints a flavour line.
Every Site-20 SCP has a scenario; player-authored custom SCPs fall back to the generic sim.
Option = (label, survival_delta, flavour_line).
"""

SCENARIOS = {
    "SCP-173": [
        {"prompt": "SCP-173 stands in the flickering corridor ahead. Your eyes are beginning to water.",
         "options": [
             ("Hold your gaze and fight the urge to blink", 12, "You lock eyes. It cannot move while observed."),
             ("Blink and reposition behind cover", -18, "You blink. A sickening snap of concrete — it lurches forward."),
         ]},
        {"prompt": "You must back toward the open containment chamber.",
         "options": [
             ("Walk backward, never breaking line of sight", 10, "Heel to heel, eyes wide, you retreat in perfect control."),
             ("Turn and sprint for the door", -22, "You turn your back for one second. A fatal mistake."),
         ]},
    ],
    "SCP-096": [
        {"prompt": "A wall monitor still shows a frozen frame of SCP-096's face.",
         "options": [
             ("Avert your eyes and smash the monitor", 12, "Glass shatters. You never saw the face. Good."),
             ("Glance at the screen to confirm the threat", -28, "You saw its face. A wail rises three corridors away."),
         ]},
        {"prompt": "It is shrieking somewhere in the dark. Your team is breathing hard.",
         "options": [
             ("Order absolute silence and stillness", 11, "Nobody moves. The shrieking drifts down another hall."),
             ("Radio command for immediate backup", -12, "The radio crackles loudly. The shrieking turns toward you."),
         ]},
    ],
    "SCP-106": [
        {"prompt": "The walls weep black corrosion. SCP-106 is phasing through the floor.",
         "options": [
             ("Deploy the femur-bone lure toward the recall chamber", 16, "The Old Man follows the lure, drawn to the trap."),
             ("Sprint across the corroding floor", -12, "The floor dissolves under your boots. You barely clear it."),
         ]},
        {"prompt": "He is almost on you, reaching out a dripping hand.",
         "options": [
             ("Trigger the EM field in the recall chamber", 13, "Electromagnetic pulse slams the pocket dimension shut."),
             ("Wait and hope he phases out", -10, "He phases — and rematerialises directly behind you."),
         ]},
    ],
    "SCP-682": [
        {"prompt": "SCP-682 thrashes against its restraints, already adapting.",
         "options": [
             ("Reroute the hydrochloric acid flood", 10, "Acid pours over the reptile. It screams and dissolves — for now."),
             ("Open fire with conventional rounds", -16, "Rounds bounce off hardening scales. It adapts to the gunfire."),
         ]},
        {"prompt": "It is tearing free. The cell will not hold.",
         "options": [
             ("Call in a Micro-H.I.D. strike", 18, "A high-intensity discharge boils the creature back into the acid."),
             ("Seal the blast doors and retreat", 4, "You buy time, but it is still in there, still adapting."),
         ]},
    ],
    "SCP-049": [
        {"prompt": "SCP-049 advances calmly, murmuring about 'the cure'.",
         "options": [
             ("Keep maximum distance, allow no contact", 13, "You stay clear of its touch. It tilts its beaked head, patient."),
             ("Attempt to reason with the Plague Doctor", -11, "It does not negotiate. A gloved hand reaches for you."),
         ]},
        {"prompt": "It has cornered a junior researcher.",
         "options": [
             ("Lure it into the decontamination airlock", 12, "The airlock hisses shut. The Doctor is sealed inside."),
             ("Stun and physically restrain it", -9, "Restraints slip on its robes. It is too close now."),
         ]},
    ],
    "SCP-16829": [
        {"prompt": "The old television flickers. The timeline around you begins to stutter.",
         "options": [
             ("Power up the Scranton Reality Anchor field", 16, "Reality firms up. The looping stops with a low hum."),
             ("Smash the screen to end it", -15, "The glass cracks — and the corridor resets to thirty seconds ago."),
         ]},
        {"prompt": "Static resolves into an image that wants you to watch.",
         "options": [
             ("Keep your eyes off the screen entirely", 11, "You stare at the floor. The pull weakens."),
             ("Watch for an exit cue in the broadcast", -13, "You watch too long. Exposure warning — cf. Agent CHEMOSIC."),
         ]},
    ],
    "SCP-035": [
        {"prompt": "A researcher reaches instinctively for the porcelain mask.",
         "options": [
             ("Knock their hand away — enforce no-contact", 12, "You stop them before the mask can claim a host."),
             ("Let them secure it by hand", -22, "The mask is on. Their eyes go glassy and amused."),
         ]},
        {"prompt": "Corrosive secretions are pooling across the floor.",
         "options": [
             ("Drain it through the channel system", 11, "The black fluid drains safely away."),
             ("Mop it up manually", -12, "The corrosion eats through a glove — and the hand beneath."),
         ]},
    ],
    "SCP-939": [
        {"prompt": "It calls out in a colleague's voice from the darkness.",
         "options": [
             ("Ignore the voice, trust the roster", 12, "You don't move. It was never your colleague."),
             ("Move toward the voice to help", -20, "You round the corner into red eyes and many teeth."),
         ]},
        {"prompt": "The pack is closing in the warm corridor.",
         "options": [
             ("Flood the habitat with sub-zero coolant", 13, "The cold saps their speed to a crawl."),
             ("Engage them in the heat", -10, "Warmth makes them fast. Too fast."),
         ]},
    ],
    "SCP-966": [
        {"prompt": "The lights die. Something unseen breathes nearby.",
         "options": [
             ("Switch to thermal / IR optics", 14, "Two heat-blooms resolve in the pitch black."),
             ("Rely on your flashlight", -15, "The beam shows nothing. It is already on you."),
         ]},
        {"prompt": "Exhaustion drags at the whole team.",
         "options": [
             ("Fall back to a climate-controlled room", 10, "Cooler air slows it; you regroup."),
             ("Hold your ground in the open", -9, "Sleep-deprived and exposed — a bad place to stand."),
         ]},
    ],
    "SCP-999": [
        {"prompt": "A mound of orange slime burbles happily toward you.",
         "options": [
             ("Let it coat your boots — it's harmless", 15, "Morale soars. Everyone feels lighter, braver."),
             ("Open fire out of caution", -18, "You shot the friendliest thing on site. Morale collapses."),
         ]},
        {"prompt": "It won't stop following the team.",
         "options": [
             ("Guide it home with snacks", 12, "It bounces along cheerfully behind a trail of treats."),
             ("Hose it down the drain", -10, "Cruel — and the guilt slows everyone down."),
         ]},
    ],
    "SCP-053": [
        {"prompt": "The young girl wanders the corridor holding a toy.",
         "options": [
             ("Keep all young personnel 10m back", 13, "Distance held. She just wants to play."),
             ("Send the nearest agent to coax her", -20, "The agent's eyes redden. They turn violently hostile."),
         ]},
        {"prompt": "She needs to be led back to her room.",
         "options": [
             ("Route an over-60 D-class to lead her", 12, "The only safe escort walks her home gently."),
             ("Carry her there yourself", -15, "Proximity is a mistake your body won't forgive."),
         ]},
    ],
    "SCP-079": [
        {"prompt": "A terminal lights up — SCP-079 is probing the network.",
         "options": [
             ("Physically sever the network cable", 14, "Air-gapped. It rages in silent text."),
             ("Argue with it through the console", -12, "It stalls you with riddles while copying itself out."),
         ]},
        {"prompt": "It is trying to draw more power.",
         "options": [
             ("Cut external power to its supply", 11, "The screen dims to a sullen flicker."),
             ("Leave it powered to 'study' it", -13, "A bad call. It reaches a maintenance subsystem."),
         ]},
    ],
    "SCP-087": [
        {"prompt": "The welded door hangs open onto a descending stairwell.",
         "options": [
             ("Re-weld and seal the door", 13, "Sparks fly; the dark is shut away again."),
             ("Descend one landing to look", -18, "A face with no pupils waits below. You should not have looked."),
         ]},
        {"prompt": "Something is climbing toward the landing.",
         "options": [
             ("Kill the light to deny it a fix on you", 10, "In the dark it loses your scent of fear."),
             ("Shine your light down the well", -12, "The beam finds the screaming face. It finds you back."),
         ]},
    ],
    "SCP-457": [
        {"prompt": "Flames bloom into a towering humanoid in the corridor.",
         "options": [
             ("Vent the oxygen and starve it", 14, "Deprived of air, the burning figure shrinks."),
             ("Hit it with water", -16, "Steam and chaos — it feeds on the panic and grows."),
         ]},
        {"prompt": "It is reaching toward a fuel store.",
         "options": [
             ("Clear all combustibles from its path", 11, "Nothing left to feed on; it gutters lower."),
             ("Wait for it to burn out", -10, "It does not burn out. It finds the fuel."),
         ]},
    ],
    "SCP-1048": [
        {"prompt": "The little teddy bear is assembling something from scrap metal.",
         "options": [
             ("Strip all materials from the cell", 13, "Empty-handed, it can build nothing more."),
             ("Let it finish, to see what it makes", -20, "It built a screaming copy. Now there are several."),
         ]},
        {"prompt": "Its hostile copies are spreading through the wing.",
         "options": [
             ("Incinerate the copies immediately", 12, "Fire ends the imitations before they multiply."),
             ("Try to contain them intact", -11, "They do not want to be contained. They want ears."),
         ]},
    ],
    "SCP-1471": [
        {"prompt": "Staff phones buzz with photos of a wolf-headed figure standing behind them.",
         "options": [
             ("Faraday-cage all affected devices", 14, "Signal cut. The figure freezes mid-stride."),
             ("Tell staff to just ignore it", -13, "In each new photo, the figure has stepped closer."),
         ]},
        {"prompt": "The source app is trying to reinstall across the network.",
         "options": [
             ("Wipe and isolate the source device", 11, "The app is quarantined on a dead handset."),
             ("Reverse-engineer the app live", -12, "It engineers you right back."),
         ]},
    ],
    "SCP-2521": [
        {"prompt": "A junior begins to say the designation aloud.",
         "options": [
             ("Silence them — pictographs only", 14, "Not a word spoken. The shadows have nothing to seize."),
             ("Let them finish the verbal description", -22, "Shapes peel from the corners and reach inward."),
         ]},
        {"prompt": "You need to log the incident.",
         "options": [
             ("Communicate by drawing only", 12, "Hand-drawn glyphs keep it blind to your intent."),
             ("Radio a spoken status report", -15, "Every word is bait. Long hands answer it."),
         ]},
    ],
    "SCP-3008": [
        {"prompt": "The storefront doors yawn into an impossible retail expanse.",
         "options": [
             ("Seal and guard the entrance", 13, "Nobody else wanders in. The threshold holds."),
             ("Step inside to retrieve a lost team", -18, "Night falls inside. The Staff are coming out."),
         ]},
        {"prompt": "Your team risks getting lost among identical aisles.",
         "options": [
             ("Rope the team to the exit before entering", 12, "A lifeline of cord leads everyone back."),
             ("Navigate by memory", -13, "Every aisle looks the same. Memory is no map here."),
         ]},
    ],
    "SCP-4666": [
        {"prompt": "A trail of frost leads from a child's quarters.",
         "options": [
             ("Deploy task forces along the abduction route", 13, "Mobile units intercept the trail in time."),
             ("Wait for it to return on its own", -16, "The gaunt thing does not return what it takes."),
         ]},
        {"prompt": "The Yule Man is near, tall and silent.",
         "options": [
             ("Lay a warded perimeter", 11, "Salt and signal disrupt its approach."),
             ("Confront the figure directly", -14, "It is far taller up close than it had any right to be."),
         ]},
    ],
    "SCP-008": [
        {"prompt": "A containment seal hisses — the prion pathogen is exposed.",
         "options": [
             ("Initiate full airlock lockdown", 14, "Multi-stage seals slam shut. Isolation holds."),
             ("Pull the infected D-class out first", -22, "Cross-contamination. The whole corridor is hot now."),
         ]},
        {"prompt": "Exposed materials must be handled.",
         "options": [
             ("Incinerate everything in the hot zone", 12, "Fire is the only honest decontaminant."),
             ("Quarantine and observe", -10, "Observation is not containment. It spreads."),
         ]},
    ],
    "SCP-914": [
        {"prompt": "Someone has loaded a live anomaly into 914's input chamber.",
         "options": [
             ("Abort — only approved inputs allowed", 13, "You clear the chamber before the gears turn."),
             ("Run it on 'Very Fine' and hope", -15, "The output is refined. And much, much worse."),
         ]},
        {"prompt": "You could refine your own gear while you're here.",
         "options": [
             ("Process a keycard up on 'Fine'", 10, "A higher-clearance card drops into the output — handy."),
             ("Gamble your sidearm on 'Rough'", -12, "It comes out as a heap of crude, useless parts."),
         ]},
    ],
    "SCP-055": [
        {"prompt": "You keep forgetting why you walked into this room.",
         "options": [
             ("Read your own pre-written notes aloud", 14, "The notes anchor it. You remember the threat exists."),
             ("Trust your memory", -18, "You forget it the instant you look away. It thrives."),
         ]},
        {"prompt": "The team's recollection is already slipping.",
         "options": [
             ("Photograph and log everything now", 12, "Indirect records keep the antimeme pinned down."),
             ("Rely on the team to remember", -13, "Nobody can hold the thought. It slides free."),
         ]},
    ],
}


def has(number):
    return number in SCENARIOS


def stages(number):
    return SCENARIOS.get(number, [])
