# Contributing Guide

Thank you for your interest in contributing to GPUPixel! This guide will help you get started with contributing to our project.

## Ways to Contribute

### Code Contributions

1. **Fork and Clone**
   - Fork the [GPUPixel repository](https://github.com/pixpark/gpupixel)
   - Clone your fork locally

2. **Create a Branch**
   - Create a new branch for your feature or fix
   - Use clear, descriptive branch names

3. **Make Changes**
   - Follow our coding style and guidelines
   - Write clear commit messages
   - Add tests if applicable

4. **Submit Pull Request**
   - Ensure your PR description clearly explains the changes
   - Link any related issues

### Documentation

- Fix typos or clarify existing documentation
- Add missing documentation for features
- Translate documentation to other languages
- Improve code examples

### Bug Reports

When reporting bugs:
- Use the [issue template](https://github.com/pixpark/gpupixel/issues/new/choose)
- Include clear steps to reproduce
- Provide system information
- Add screenshots if applicable

### Feature Requests

When suggesting features:
- Explain the problem you're trying to solve
- Describe the feature and its benefits
- Consider implementation complexity

## Development Setup

1. **Prerequisites**
   - C++11 compiler
   - CMake 3.10+
   - OpenGL/ES development environment

2. **Build Instructions**
   
```bash
   git clone https://github.com/YOUR_USERNAME/gpupixel.git
   cd gpupixel
   mkdir build && cd build
   cmake ..
   make
```

## Code Style Guidelines

We follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with some modifications:

### General Rules
- Use C++11 features appropriately
- Follow existing code formatting
- Add comments for complex logic
- Keep functions focused and concise

### Specific Guidelines
- Use 4 spaces for indentation, not tabs
- Maximum line length is 100 characters
- Use `camelCase` for variable and function names
- Use `PascalCase` for class names
- Use `UPPER_CASE` for constants and macros
- Place opening braces on the same line
- Add spaces around operators and after commas

### Example
```cpp
class MyClass {
public:
    void doSomething(const std::string& inputParam) {
        if (inputParam.empty()) {
            return;
        }
        
        int someValue = calculateValue(inputParam);
        processResult(someValue);
    }
    
private:
    static const int MAX_VALUE = 100;
    std::string className_;
};
```

## Testing
- Ensure existing tests pass
- Test on multiple platforms if possible
## Community Guidelines
- Be respectful and inclusive
- Help others in discussions
- Follow our code of conduct
- Give credit where due
## Getting Help
- Join our Discord community
- Ask questions in GitHub Discussions
- Contact maintainers via email
## License
By contributing to GPUPixel, you agree that your contributions will be licensed under the MIT License .