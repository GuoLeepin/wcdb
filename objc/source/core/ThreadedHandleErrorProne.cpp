/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/HandlePool.hpp>
#include <WCDB/ThreadedHandleErrorProne.hpp>

namespace WCDB {

ThreadLocal<ThreadedHandleErrorProne::ThreadedErrors> *
ThreadedHandleErrorProne::threadedErrors()
{
    static ThreadLocal<ThreadedErrors> s_threadedErrors;
    return &s_threadedErrors;
}

void ThreadedHandleErrorProne::setThreadedError(const Error &error) const
{
    (*threadedErrors()->get())[getErrorAssociatedHandlePool()] = error;
}

void ThreadedHandleErrorProne::setThreadedError(Error &&error) const
{
    (*threadedErrors()->get())[getErrorAssociatedHandlePool()] =
        std::move(error);
}

const Error &ThreadedHandleErrorProne::getThreadedError() const
{
    return (*threadedErrors()->get())[getErrorAssociatedHandlePool()];
}

void ThreadedHandleErrorProne::error(Error &&error) const
{
    const HandlePool *handlePool = getErrorAssociatedHandlePool();
    if (handlePool->getTag() != Handle::invalidTag) {
        error.infos.set("Tag", handlePool->getTag());
    }
    error.infos.set("Path", handlePool->path);
    Reporter::shared()->report(error);
    setThreadedError(std::move(error));
}

} //namespace WCDB
