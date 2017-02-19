#ifndef IVISITABLE_H
#define IVISITABLE_H

class IVisitor;
class IVisitable
{
    public:
        /** Default constructor */
        IVisitable() {}
        /** Default destructor */
        virtual ~IVisitable() {}
        virtual void accept(IVisitor&) = 0;
};

#endif // IVISITABLE_H
