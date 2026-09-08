#pragma once

#import "luna.h"

namespace vax::engine {
class InderectDrawController final {
  public:
    InderectDrawController() {};

    ~InderectDrawController() = default;

    void draw();

    void pushCommand(VkDrawIndexedIndirectCommand command);

  private:
    std::vector<VkDrawIndexedIndirectCommand> _commands;
};
} // namespace vax::engine