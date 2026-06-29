"""RNG helpers backed by the cryptographically-secure `secrets` module."""
import secrets


def roll(low, high):
    """Inclusive integer in [low, high]."""
    return secrets.randbelow(high - low + 1) + low


def choice(seq):
    return secrets.choice(seq)


def chance(percent):
    """True with `percent`% probability."""
    return secrets.randbelow(100) < percent
