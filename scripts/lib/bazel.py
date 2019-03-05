import os

from lib.command_composer import CommandComposer
from lib.error import invalud_argument, not_reached


class Package(object):
    def __init__(self, name, all = False):
        """Constructor.

        Args:
            name: package name
            all: whether contains all the targets beneath package
        """
        if name.startswith('//'):
            name = name[2:]
        if name.endswith('/'):
            name = name[:-1]
        self.name = name
        self.all = all

    def is_target(self):
        """Whether it is a target"""
        return self.all

    def __str__(self):
        if self.all:
            return '//{}/...'.format(self.name)
        return '//{}'.format(self.name)


class Target(object):
    def __init__(self, package, name):
        """Constructor.

        Args:
            package: package instance
            name: target name
        """
        if not isinstance(package, Package):
            invalud_argument("package is not Package type")
        if package.all:
            invalud_argument("package whose all is set can't be built to target")
        self.package = package
        self.name = name

    @staticmethod
    def from_str(target_str):
        """Construct Target instance from str."""
        sep = target_str.find(':')
        if sep == -1:
            invalud_argument('"{}" is not valid target str'.format(target_str))
        package = Package(target_str[:sep])
        return Target(package, target_str[sep + 1:])

    def is_target(self):
        """Whether it is a target"""
        return True

    def __str__(self):
        return '{}:{}'.format(self.package, self.name)


def requires_target(name, target):
    """Check whether target is instance of Package or Target

    Raises:
        ValueError: If check fails.
    """
    if isinstance(target, (Package, Target)):
        if target.is_target():
            return
    invalud_argument("{} should be package or target".format(name))


class QueryCommandOperand(object):
    DEPS = 0
    KIND = 1
    SOMEPATH = 2
    ATTR = 3

    def __init__(self, kind, **kargs):
        """Constructor.

        Args:
            kind: DEPS, KIND or SOMEPATH
        """
        kinds = [QueryCommandOperand.DEPS, QueryCommandOperand.KIND, QueryCommandOperand.SOMEPATH,
                 QueryCommandOperand.ATTR]
        if kind not in kinds:
            invalud_argument("query target is not in {}".format(['DEPS', 'KIND', 'SOMEPATH', 'ATTR']))
        self.__dict__['kind'] = kind
        for k, v in kargs.items():
            setattr(self, k, v)

    @staticmethod
    def deps(**kargs):
        return QueryCommandOperand(QueryCommandOperand.DEPS, **kargs)

    @staticmethod
    def kind(**kargs):
        return QueryCommandOperand(QueryCommandOperand.KIND, **kargs)

    @staticmethod
    def somepath(**kargs):
        return QueryCommandOperand(QueryCommandOperand.SOMEPATH, **kargs)

    @staticmethod
    def attr(**kargs):
        return QueryCommandOperand(QueryCommandOperand.ATTR, **kargs)

    def is_target(self):
        """Whether it is a target"""
        return True

    def __setattr__(self, name, value):
        """Validate attributes."""
        if name == 'target':
            requires_target(name, value)
            super().__setattr__(name, value)
            return

        if self.kind == QueryCommandOperand.KIND:
            if name == 'label':
                super().__setattr__(name, value)
                return
        elif self.kind == QueryCommandOperand.SOMEPATH:
            if name == 'dependancy':
                requires_target(name, value)
                super().__setattr__(name, value)
                return
        elif self.kind == QueryCommandOperand.ATTR:
            if name == 'key' or name == 'value':
                super().__setattr__(name, value)
                return

        invalud_argument(name)

    def __str__(self):
        if self.kind == QueryCommandOperand.DEPS:
            return 'deps("{}")'.format(self.target)
        elif self.kind == QueryCommandOperand.KIND:
            return 'kind("{}", "{}")'.format(self.label, self.target)
        elif self.kind == QueryCommandOperand.SOMEPATH:
            return 'somepath("{}", "{}")'.format(self.target, self.dependancy)
        elif self.kind == QueryCommandOperand.ATTR:
            return 'attr("{}", "{}", "{}")'.format(self.key, self.value, self.target)
        not_reached()


class QueryCommandExpr(object):
    INTERSECTION = 0
    EXCEPT = 1

    def __init__(self, target):
        assert target.is_target()
        self.exprs = [target]

    def add(self, target, operation):
        assert target.is_target()
        target.__dict__['operation'] = operation
        self.exprs.append(target)

    def add_intersect(self, operand):
        return self.add(operand, QueryCommandExpr.INTERSECTION)

    def add_except(self, operand):
        return self.add(operand, QueryCommandExpr.EXCEPT)

    def __str__(self):
        cmd = []
        for expr in self.exprs:
            if not hasattr(expr, 'operation'):
                cmd.append(str(expr))
            elif expr.operation == QueryCommandExpr.INTERSECTION:
                cmd.extend(['intersect', str(expr)])
            elif expr.operation == QueryCommandExpr.EXCEPT:
                cmd.extend(['except', str(expr)])
        return ' '.join(cmd)


class Bazel(CommandComposer):
    def __init__(self):
        """Constructor."""
        super().__init__('bazel')

    def build(self, options, target):
        """Build target with options."""
        if isinstance(target, Target):
            target = str(target)
        cmd = self.compose('build', options, target)
        return self.run_and_check_returncode(cmd)

    def test(self, options, target):
        """Test target with options."""
        if isinstance(target, Target):
            target = str(target)
        cmd = self.compose('test', options, target)
        return self.run_and_check_returncode(cmd)

    def run(self, options, target):
        """Run target with options."""
        if isinstance(target, Target):
            target = str(target)
        cmd = self.compose('run', options, target)
        return self.run_and_check_returncode(cmd)

    def query(self, options, stmt):
        """Query statement with options."""
        if isinstance(stmt, (QueryCommandOperand, QueryCommandExpr)):
            stmt = str(stmt)
        cmd = self.compose('query', options, stmt)
        return self.run_and_check_returncode(cmd)