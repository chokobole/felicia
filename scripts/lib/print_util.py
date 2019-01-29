class PrintUtil(object):
    RED='\033[91m'
    GREEN='\033[32m'
    BLUE='\033[34m'
    YELLOW='\033[93m'
    BOLD='\033[1m'
    NONE='\033[0m'

    @staticmethod
    def print_red(stmt):
        print('{}{}{}'.format(PrintUtil.RED, stmt, PrintUtil.NONE))

    @staticmethod
    def print_green(stmt):
        print('{}{}{}'.format(PrintUtil.GREEN, stmt, PrintUtil.NONE))

    @staticmethod
    def print_blue(stmt):
        print('{}{}{}'.format(PrintUtil.BLUE, stmt, PrintUtil.NONE))

    @staticmethod
    def print_yellow(stmt):
        print('{}{}{}'.format(PrintUtil.YELLOW, stmt, PrintUtil.NONE))

    @staticmethod
    def print_bold(stmt):
        print('{}{}{}'.format(PrintUtil.BOLD, stmt, PrintUtil.NONE))