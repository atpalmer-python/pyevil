import bastardize


def test_int_copy():
    """
    If you're mutating ints, you might want copies...
    """
    newone = bastardize.int_copy(1)
    assert newone == 1
    assert not (newone is 1)


def test_int_copy_neg():
    """
    Because Python has a weird representation for negative ints,
    test this, too...
    """
    someneg = bastardize.int_copy(-10)
    assert someneg == -10
    assert not (someneg is -10)


def test_int_mutate():
    one = bastardize.int_copy(1)
    assert one == 1
    bastardize.int_mutate(one, 2)
    assert one != 1
    assert one == 2
    assert one + one == 4


def test_evil_int_mutate():
    """
    Not impressed with mutating copies?
    Why not mutate literals?
    (If you're bold, try changing the value of 1 and see how many libraries you can break.)
    """
    assert 800 != 900
    bastardize.int_mutate(800, 900, force=True)
    assert 800 == 900
    assert 400 + 400 != 800 # *evil laugh*


def test_float_mutate():
    """
    Let's mess with floats, too.
    """
    bastardize.float_mutate(2.0, 1.5)
    assert 2.0 * 2 == 3


def test_float_mutate_copy():
    """
    We can use copies, too...
    """
    three = bastardize.float_copy(3.0)
    still_three = three
    bastardize.float_mutate(three, 4.0)
    # We've taken a float object and changed its value.
    # This is different from reassigning to a new object,
    # On the surface, it looks kind of the same as `three = 4.0`
    assert three != 3.0
    assert three == 4
    # but all references were updated
    assert still_three == 4


def test_tuple_set_item():
    """
    If only you could do this with tuples...
        data[0] = 4 # Gives TypeError: 'tuple' object does not support item assignment
    """
    data = (1,2,3)
    bastardize.tuple_set_item(data, index=0, value=4)
    assert data == (4,2,3)
