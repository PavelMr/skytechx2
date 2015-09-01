#include "skdebug.h"
#include "skmath.h"

QDebug operator<<(QDebug debug, const RaDec &rd)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "RaDec " << SkMath::toDeg(rd.ra) << " " << SkMath::toDeg(rd.dec);

    return debug;
}
