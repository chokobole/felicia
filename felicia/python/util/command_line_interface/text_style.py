class TextStyle(object):
    RED='\033[91m'
    GREEN='\033[32m'
    BLUE='\033[34m'
    YELLOW='\033[93m'
    BOLD='\033[1m'
    NONE='\033[0m'

    @staticmethod
    def red(stmt):
        return '{}{}{}'.format(TextStyle.RED, stmt, TextStyle.NONE)

    @staticmethod
    def green(stmt):
        return '{}{}{}'.format(TextStyle.GREEN, stmt, TextStyle.NONE)

    @staticmethod
    def blue(stmt):
        return '{}{}{}'.format(TextStyle.BLUE, stmt, TextStyle.NONE)

    @staticmethod
    def yellow(stmt):
        return '{}{}{}'.format(TextStyle.YELLOW, stmt, TextStyle.NONE)

    @staticmethod
    def bold(stmt):
        return '{}{}{}'.format(TextStyle.BOLD, stmt, TextStyle.NONE)

    @staticmethod
    def print_error(stmt):
        print('{} {}'.format(TextStyle.red('Error:'), stmt))