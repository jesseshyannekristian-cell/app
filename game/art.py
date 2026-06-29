"""ASCII art for anomalies, shown during breach response. Rendered via rich.Text (no markup)."""

_ART = {
    "SCP-173": r"""
       .-=-=-.
      /  _ _  \
      | (o)(o) |
      \   ^    /
       )_____(
      /| | | |\
     / | | | | \
       |_|_|_|
      concrete + rebar
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
