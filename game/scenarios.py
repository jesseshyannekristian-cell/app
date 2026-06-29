"""Per-SCP branching breach mini-scenarios.

Each scenario is a list of decision stages. Each stage offers options; choosing one
applies a delta to the running survival chance and prints a flavour line.
SCPs not listed here fall back to the generic breach simulation.
"""

SCENARIOS = {
    "SCP-173": [
        {
            "prompt": "SCP-173 stands in the flickering corridor ahead. Your eyes are beginning to water.",
            "options": [
                ("Hold your gaze and fight the urge to blink", 12,
                 "You lock eyes. It cannot move while observed. Stalemate."),
                ("Blink and reposition behind cover", -18,
                 "You blink. A sickening snap of concrete — it lurches forward."),
            ],
        },
        {
            "prompt": "You must back toward the open containment chamber.",
            "options": [
                ("Walk backward, never breaking line of sight", 10,
                 "Heel to heel, eyes wide, you retreat in perfect control."),
                ("Turn and sprint for the door", -22,
                 "You turn your back for one second. A fatal mistake."),
            ],
        },
    ],
    "SCP-096": [
        {
            "prompt": "A wall monitor still shows a frozen frame of SCP-096's face.",
            "options": [
                ("Avert your eyes and smash the monitor", 12,
                 "Glass shatters. You never saw the face. Good."),
                ("Glance at the screen to confirm the threat", -28,
                 "You saw its face. A wail rises from three corridors away."),
            ],
        },
        {
            "prompt": "It is shrieking somewhere in the dark. Your team is breathing hard.",
            "options": [
                ("Order absolute silence and stillness", 11,
                 "Nobody moves. The shrieking drifts away down another hall."),
                ("Radio command for immediate backup", -12,
                 "The radio crackles loudly. The shrieking turns toward you."),
            ],
        },
    ],
    "SCP-106": [
        {
            "prompt": "The walls weep black corrosion. SCP-106 is phasing through the floor.",
            "options": [
                ("Deploy the femur-bone lure toward the recall chamber", 16,
                 "The Old Man follows the lure, drawn to the trap."),
                ("Sprint across the corroding floor", -12,
                 "The floor dissolves under your boots. You barely clear it."),
            ],
        },
        {
            "prompt": "He is almost on you, reaching out a dripping hand.",
            "options": [
                ("Trigger the EM field in the recall chamber", 13,
                 "Electromagnetic pulse slams the pocket dimension shut."),
                ("Wait and hope he phases out", -10,
                 "He phases — and rematerialises directly behind you."),
            ],
        },
    ],
    "SCP-682": [
        {
            "prompt": "SCP-682 thrashes against its restraints, already adapting.",
            "options": [
                ("Reroute the hydrochloric acid flood", 10,
                 "Acid pours over the reptile. It screams and dissolves — for now."),
                ("Open fire with conventional rounds", -16,
                 "Rounds bounce off hardening scales. It adapts to the gunfire."),
            ],
        },
        {
            "prompt": "It is tearing free. The cell will not hold.",
            "options": [
                ("Call in a Micro-H.I.D. strike", 18,
                 "A high-intensity discharge boils the creature back into the acid."),
                ("Seal the blast doors and retreat", 4,
                 "You buy time, but it is still in there, still adapting."),
            ],
        },
    ],
    "SCP-049": [
        {
            "prompt": "SCP-049 advances calmly, murmuring about 'the cure'.",
            "options": [
                ("Keep maximum distance, allow no contact", 13,
                 "You stay clear of its touch. It tilts its beaked head, patient."),
                ("Attempt to reason with the Plague Doctor", -11,
                 "It does not negotiate. A gloved hand reaches for you."),
            ],
        },
        {
            "prompt": "It has cornered a junior researcher.",
            "options": [
                ("Lure it into the decontamination airlock", 12,
                 "The airlock hisses shut. The Doctor is sealed inside."),
                ("Stun and physically restrain it", -9,
                 "Restraints slip on its robes. It is too close now."),
            ],
        },
    ],
    "SCP-16829": [
        {
            "prompt": "The old television flickers. The timeline around you begins to stutter.",
            "options": [
                ("Power up the Scranton Reality Anchor field", 16,
                 "Reality firms up. The looping stops with a low hum."),
                ("Smash the screen to end it", -15,
                 "The glass cracks — and the corridor resets to thirty seconds ago."),
            ],
        },
        {
            "prompt": "Static resolves into an image that wants you to watch.",
            "options": [
                ("Keep your eyes off the screen entirely", 11,
                 "You stare at the floor. The pull weakens."),
                ("Watch for an exit cue in the broadcast", -13,
                 "You watch too long. Cellular exposure warning — cf. Agent CHEMOSIC."),
            ],
        },
    ],
}


def has(number):
    return number in SCENARIOS


def stages(number):
    return SCENARIOS.get(number, [])
