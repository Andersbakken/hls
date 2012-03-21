#include <QtCore>

void printLine(const QString &line, int col)
{
    printf("%s\n", qPrintable(line));
    if (col) {
        for (int i=1; i<col; ++i) {
            printf(" ");
        }
        printf("^\n");
    }
}

int main(int argc, char **argv)
{
    QRegExp lineColRx("^(.*):([0-9]+):([0-9]+):?$");
    QRegExp lineRx("^(.*):([0-9]+):?$");
    QRegExp offsetRx("^(.*),([0-9]+)$");
    bool context = false;
    for (int i=1; i<argc; ++i) {
        if (!strcmp(argv[i], "--context") || !strcmp(argv[i], "-c")) {
            context = true;
            continue;
        }
        int off = 0;
        int line = 0;
        int col = 0;
        QString fileName;
        if (lineColRx.exactMatch(argv[i])) {
            fileName = lineColRx.cap(1);
            line = lineColRx.cap(2).toInt();
            if (!line)
                return 1;
            col = lineColRx.cap(3).toInt();
            if (!col)
                return 1;
        } else if (lineRx.exactMatch(argv[i])) {
            fileName = lineRx.cap(1);
            line = lineRx.cap(2).toInt();
            if (!line)
                return 1;
        } else if (offsetRx.exactMatch(argv[i])) {
            fileName = offsetRx.cap(1);
            off = offsetRx.cap(2).toInt();
            if (!off)
                return 1;
            ++off;
        } else {
            return 1;
        }
        QFile f(fileName);
        if (!f.open(QIODevice::ReadOnly))
            return 1;
        QTextStream ts(&f);
        QString l;
        QString last, secondLast;
        if (line) {
            while (!ts.atEnd()) {
                secondLast = last;
                last = l;
                l = ts.readLine();
                if (!--line)
                    break;
            }
            if (line)
                return 1;
            if (context) {
                printLine(secondLast, 0);
                printLine(last, 0);
            }
            printLine(l, col);
            if (context) {
                for (int i=0; i<2 && !ts.atEnd(); ++i) {
                    printLine(ts.readLine(), 0);
                }
            }
        } else {
            forever {
                secondLast = last;
                last = l;
                l = ts.readLine();
                if (off > l.size() + 1) {
                    off -= (l.size() + 1);
                } else {
                    break;
                }
                if (ts.atEnd())
                    return 1;
            }
            if (context) {
                printLine(secondLast, 0);
                printLine(last, 0);
            }
            printLine(l, off);
            if (context) {
                for (int i=0; i<2 && !ts.atEnd(); ++i) {
                    printLine(ts.readLine(), 0);
                }
            }
        }
    }
    return 0;
}
