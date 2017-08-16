import os

import re


def walkDir(d):
    for root, dirs, files in os.walk(d):
        for f in files:
            yield '%s/%s' % (root, f)


def _clamp(n, minVal, maxVal):
    if n < minVal:
        return minVal
    if n > maxVal:
        return maxVal
    return n


def _filter(items, Type):
    for item in items:
        if isinstance(item, Type):
            yield item


def _excludeTypes(items, Types):
    filteredItems = []
    for item in items:
        exclude = False;
        for itemType in Types:
            if isinstance(item, itemType):
                exclude = True
        if not exclude:
            filteredItems.append(item)
    return filteredItems


def allSubClasses(cls):
    """ Retrieve all subclasses of the specified type. Results may vary depending on what is imported. """
    all_subclasses = []

    for subclass in cls.__subclasses__():
        all_subclasses.append(subclass)
        all_subclasses.extend(allSubClasses(subclass))

    return reversed(all_subclasses)


def fuzzyfinder(user_input, collection):
    suggestions = []
    pattern = '.*?'.join(user_input)  # Converts 'djm' to 'd.*?j.*?m'
    regex = re.compile(pattern)  # Compiles a regex.
    for item in collection:
        match = regex.search(item)  # Checks if the current item matches the regex.
        if match:
            suggestions.append((len(match.group()), match.start(), item))
    return [x for _, _, x in sorted(suggestions)]


def fuzzymatches(input, other):
    pattern = '.*?'.join(input)  # Converts 'djm' to 'd.*?j.*?m'
    regex = re.compile(pattern)  # Compiles a regex.
    return bool(regex.search(other))
