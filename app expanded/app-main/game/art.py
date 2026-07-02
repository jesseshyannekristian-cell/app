"""ASCII art for anomalies, shown during breach response. Rendered via rich.Text (no markup).

ALL ASCII art below is ORIGINAL, fan-made work created for this project and released under
CC BY-SA 3.0. None of it is traced from or derived from any photograph or copyrighted
depiction of any SCP — in particular the SCP-173 art is an abstract, stylised original and is
NOT based on the "Untitled 2004" sculpture historically associated with SCP-173. See CREDITS.md.
"""

_ART = {
    "SCP-173": r"""
        ______
       /  __  \
      |  |##|  |
      |  |##|  |    SCP-173
       \ |##| /     (original fan-made
        \|##|/       ASCII art)
         |##|
        _|##|_
       /______\
      do not blink
""",
    "SCP-096": r"""
        _______
       /       \
      |  .   .  |
      ||       ||
      |  \___/  |   <- DO NOT
       \_______/      VIEW FACE
      /| | | | |\
""",
    "SCP-106": r"""
       .~~~~~-.
     .: . . . :.
    /   (o)(o)   \
    |    \__/    |
     \  corrode /
      .-_____-.
     drip . drip
""",
    "SCP-682": r"""
      /\_/\_/\
     < ==  == >---,
      \__ ^ __/   /
       | | | |   /
       =-=-=----=
     hard-to-destroy
""",
    "SCP-049": r"""
        ___
      .:   :.
     / o   o \
     |  ===  |   the
     |  beak |   cure
      \_____/
       |   |
""",
    "SCP-16829": r"""
      .----------.
      | ~ ~ ~ ~  |
      |  N O      |
      |  SIGNAL   |
      .----------.
        ||    ||
     temporal loop
""",
}

_GENERIC = r"""
       ______
      / ???? \
     | ?SCP? |
     | ????? |
      \______/
     anomalous object
"""


def for_scp(number):
    return _ART.get(number, _GENERIC)
