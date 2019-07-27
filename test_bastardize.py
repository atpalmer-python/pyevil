import pytest
import bastardize


def test_get_refcnt():
    hopefully_unused = 10983647581
    refcnt = bastardize.get_refcnt(hopefully_unused)
    assert refcnt == 3


def test_int_copy():
    """
    If you're mutating ints, you might want copies...
    """
    newone = bastardize.int_copy(1)
    assert newone == 1
    assert bastardize.get_refcnt(newone) == 2
    assert bastardize.get_refcnt(1) > 2


def test_int_mutate():
    copy = bastardize.int_copy(42)
    assert copy == 42
    bastardize.int_mutate(copy, 99)
    assert copy == 99
    assert copy != 42


def test_tuple_set_item():
    """
    If only you could do this with tuples...
        data[0] = 4 # Gives TypeError: 'tuple' object does not support item assignment
    """
    data = (1,2,3)
    bastardize.tuple_set_item(data, index=0, value=4)
    assert data == (4,2,3)
