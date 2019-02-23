def invalud_argument(text):
    raise ValueError('Invalud Argument: {}'.format(text))

def not_reached():
    raise RuntimeError('Should not reached')