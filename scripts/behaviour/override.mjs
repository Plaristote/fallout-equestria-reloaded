export function overrideBehaviour(object, method, override) {
  const originalMethod = object[method] ? object[method].bind(object) : null;

  object[method] = (...args) => {
    if (override(...args))
      return true;
    return originalMethod ? originalMethod(...args) : false;
  };
}
