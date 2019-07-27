import pytest
import bastardize


def test_int_mutate():
    """
    Doing this is reassigning an int's value.
    ints are used for lots of things in the libraries you import--beware...
    """
    assert 42 != 99
    bastardize.int_mutate(42, 99)
    assert 42 == 99
    assert 10 + 32 == 99 # even this works


def test_tuple_set_item():
    """
    If only you could do this with tuples...
        data[0] = 4 # Gives TypeError: 'tuple' object does not support item assignment
    """
    data = (1,2,3)
    bastardize.tuple_set_item(data, index=0, value=4)
    assert data == (4,2,3)
