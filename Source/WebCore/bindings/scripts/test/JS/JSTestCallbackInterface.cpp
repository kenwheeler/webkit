/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(TEST_CONDITIONAL)

#include "JSTestCallbackInterface.h"

#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertBoolean.h"
#include "JSDOMConvertBufferSource.h"
#include "JSDOMConvertInterface.h"
#include "JSDOMConvertNumbers.h"
#include "JSDOMConvertSerializedScriptValue.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMGlobalObject.h"
#include "JSDOMStringList.h"
#include "JSTestNode.h"
#include "ScriptExecutionContext.h"
#include "SerializedScriptValue.h"
#include <runtime/FunctionPrototype.h>
#include <runtime/JSCInlines.h>
#include <runtime/JSLock.h>
#include <runtime/JSString.h>
#include <wtf/NeverDestroyed.h>

using namespace JSC;

namespace WebCore {

template<> JSString* convertEnumerationToJS(ExecState& state, TestCallbackInterface::Enum enumerationValue)
{
    static const NeverDestroyed<String> values[] = {
        MAKE_STATIC_STRING_IMPL("value1"),
        MAKE_STATIC_STRING_IMPL("value2"),
    };
    static_assert(static_cast<size_t>(TestCallbackInterface::Enum::Value1) == 0, "TestCallbackInterface::Enum::Value1 is not 0 as expected");
    static_assert(static_cast<size_t>(TestCallbackInterface::Enum::Value2) == 1, "TestCallbackInterface::Enum::Value2 is not 1 as expected");
    ASSERT(static_cast<size_t>(enumerationValue) < WTF_ARRAY_LENGTH(values));
    return jsStringWithCache(&state, values[static_cast<size_t>(enumerationValue)]);
}

template<> std::optional<TestCallbackInterface::Enum> parseEnumeration<TestCallbackInterface::Enum>(ExecState& state, JSValue value)
{
    auto stringValue = value.toWTFString(&state);
    if (stringValue == "value1")
        return TestCallbackInterface::Enum::Value1;
    if (stringValue == "value2")
        return TestCallbackInterface::Enum::Value2;
    return std::nullopt;
}

template<> const char* expectedEnumerationValues<TestCallbackInterface::Enum>()
{
    return "\"value1\", \"value2\"";
}

template<> TestCallbackInterface::Dictionary convertDictionary<TestCallbackInterface::Dictionary>(ExecState& state, JSValue value)
{
    VM& vm = state.vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    bool isNullOrUndefined = value.isUndefinedOrNull();
    auto* object = isNullOrUndefined ? nullptr : value.getObject();
    if (UNLIKELY(!isNullOrUndefined && !object)) {
        throwTypeError(&state, throwScope);
        return { };
    }
    TestCallbackInterface::Dictionary result;
    JSValue optionalMemberValue = isNullOrUndefined ? jsUndefined() : object->get(&state, Identifier::fromString(&state, "optionalMember"));
    if (!optionalMemberValue.isUndefined()) {
        result.optionalMember = convert<IDLLong>(state, optionalMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue requiredMemberValue = isNullOrUndefined ? jsUndefined() : object->get(&state, Identifier::fromString(&state, "requiredMember"));
    if (!requiredMemberValue.isUndefined()) {
        result.requiredMember = convert<IDLUSVString>(state, requiredMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    } else {
        throwRequiredMemberTypeError(state, throwScope, "requiredMember", "TestCallbackInterfaceDictionary", "USVString");
        return { };
    }
    return result;
}

JSTestCallbackInterface::JSTestCallbackInterface(JSObject* callback, JSDOMGlobalObject* globalObject)
    : TestCallbackInterface()
    , ActiveDOMCallback(globalObject->scriptExecutionContext())
    , m_data(new JSCallbackDataStrong(callback, globalObject, this))
{
}

JSTestCallbackInterface::~JSTestCallbackInterface()
{
    ScriptExecutionContext* context = scriptExecutionContext();
    // When the context is destroyed, all tasks with a reference to a callback
    // should be deleted. So if the context is 0, we are on the context thread.
    if (!context || context->isContextThread())
        delete m_data;
    else
        context->postTask(DeleteCallbackDataTask(m_data));
#ifndef NDEBUG
    m_data = nullptr;
#endif
}

using JSTestCallbackInterfaceConstructor = JSDOMConstructorNotConstructable<JSTestCallbackInterface>;

/* Hash table for constructor */

static const HashTableValue JSTestCallbackInterfaceConstructorTableValues[] =
{
    { "CONSTANT1", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, { (long long)(1) } },
    { "CONSTANT2", DontDelete | ReadOnly | ConstantInteger, NoIntrinsic, { (long long)(2) } },
};

static_assert(TestCallbackInterface::CONSTANT1 == 1, "CONSTANT1 in TestCallbackInterface does not match value from IDL");
static_assert(TestCallbackInterface::CONSTANT2 == 2, "CONSTANT2 in TestCallbackInterface does not match value from IDL");

template<> JSValue JSTestCallbackInterfaceConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestCallbackInterfaceConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(globalObject);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String(ASCIILiteral("TestCallbackInterface"))), ReadOnly | DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontEnum);
    reifyStaticProperties(vm, JSTestCallbackInterfaceConstructorTableValues, *this);
}

template<> const ClassInfo JSTestCallbackInterfaceConstructor::s_info = { "TestCallbackInterface", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestCallbackInterfaceConstructor) };

JSValue JSTestCallbackInterface::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestCallbackInterfaceConstructor>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

CallbackResult<typename IDLVoid::ImplementationType> JSTestCallbackInterface::callbackWithNoParam()
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;

    NakedPtr<JSC::Exception> returnedException;
    m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackWithNoParam"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    return { };
}

CallbackResult<typename IDLVoid::ImplementationType> JSTestCallbackInterface::callbackWithArrayParam(typename IDLFloat32Array::ParameterType arrayParam)
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLFloat32Array>(state, globalObject, arrayParam));

    NakedPtr<JSC::Exception> returnedException;
    m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackWithArrayParam"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    return { };
}

CallbackResult<typename IDLVoid::ImplementationType> JSTestCallbackInterface::callbackWithSerializedScriptValueParam(typename IDLSerializedScriptValue<SerializedScriptValue>::ParameterType srzParam, typename IDLDOMString::ParameterType strParam)
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLSerializedScriptValue<SerializedScriptValue>>(state, globalObject, srzParam));
    args.append(toJS<IDLDOMString>(state, strParam));

    NakedPtr<JSC::Exception> returnedException;
    m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackWithSerializedScriptValueParam"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    return { };
}

CallbackResult<typename IDLVoid::ImplementationType> JSTestCallbackInterface::callbackWithStringList(typename IDLInterface<DOMStringList>::ParameterType listParam)
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLInterface<DOMStringList>>(state, globalObject, listParam));

    NakedPtr<JSC::Exception> returnedException;
    m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackWithStringList"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    return { };
}

CallbackResult<typename IDLVoid::ImplementationType> JSTestCallbackInterface::callbackWithBoolean(typename IDLBoolean::ParameterType boolParam)
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLBoolean>(boolParam));

    NakedPtr<JSC::Exception> returnedException;
    m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackWithBoolean"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    return { };
}

CallbackResult<typename IDLVoid::ImplementationType> JSTestCallbackInterface::callbackRequiresThisToPass(typename IDLLong::ParameterType longParam, typename IDLInterface<TestNode>::ParameterType testNodeParam)
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLLong>(longParam));
    args.append(toJS<IDLInterface<TestNode>>(state, globalObject, testNodeParam));

    NakedPtr<JSC::Exception> returnedException;
    m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackRequiresThisToPass"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    return { };
}

CallbackResult<typename IDLDOMString::ImplementationType> JSTestCallbackInterface::callbackWithAReturnValue()
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;

    NakedPtr<JSC::Exception> returnedException;
    auto jsResult = m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackWithAReturnValue"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto returnValue = convert<IDLDOMString>(state, jsResult);
    RETURN_IF_EXCEPTION(throwScope, CallbackResultType::ExceptionThrown);
    return WTFMove(returnValue);
}

CallbackResult<typename IDLDOMString::ImplementationType> JSTestCallbackInterface::callbackThatRethrowsExceptions(typename IDLEnumeration<TestCallbackInterface::Enum>::ParameterType enumParam)
{
    if (!canInvokeCallback())
        return CallbackResultType::UnableToExecute;

    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLEnumeration<TestCallbackInterface::Enum>>(state, enumParam));

    NakedPtr<JSC::Exception> returnedException;
    auto jsResult = m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackThatRethrowsExceptions"), returnedException);
    if (returnedException) {
        auto throwScope = DECLARE_THROW_SCOPE(vm);
        throwException(&state, throwScope, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto returnValue = convert<IDLDOMString>(state, jsResult);
    RETURN_IF_EXCEPTION(throwScope, CallbackResultType::ExceptionThrown);
    return WTFMove(returnValue);
}

CallbackResult<typename IDLDOMString::ImplementationType> JSTestCallbackInterface::callbackThatSkipsInvokeCheck(typename IDLDictionary<TestCallbackInterface::Dictionary>::ParameterType dictionaryParam)
{
    Ref<JSTestCallbackInterface> protectedThis(*this);

    auto& globalObject = *m_data->globalObject();
    auto& vm = globalObject.vm();

    JSLockHolder lock(vm);
    auto& state = *globalObject.globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS<IDLDictionary<TestCallbackInterface::Dictionary>>(state, globalObject, dictionaryParam));

    NakedPtr<JSC::Exception> returnedException;
    auto jsResult = m_data->invokeCallback(args, JSCallbackData::CallbackType::Object, Identifier::fromString(&vm, "callbackThatSkipsInvokeCheck"), returnedException);
    if (returnedException) {
        reportException(&state, returnedException);
        return CallbackResultType::ExceptionThrown;
     }

    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto returnValue = convert<IDLDOMString>(state, jsResult);
    RETURN_IF_EXCEPTION(throwScope, CallbackResultType::ExceptionThrown);
    return WTFMove(returnValue);
}

JSC::JSValue toJS(TestCallbackInterface& impl)
{
    if (!static_cast<JSTestCallbackInterface&>(impl).callbackData())
        return jsNull();

    return static_cast<JSTestCallbackInterface&>(impl).callbackData()->callback();

}

} // namespace WebCore

#endif // ENABLE(TEST_CONDITIONAL)
